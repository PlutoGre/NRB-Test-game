using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Web.Script.Serialization;
using System.Windows.Forms;

namespace NrbAdmin
{
    /// <summary>Windows 10 及以上的本机游戏管理入口；不启动或改写未就绪的服务。</summary>
    internal static class Program
    {
        [STAThread]
        private static void Main(string[] args)
        {
            using (var mutex = new System.Threading.Mutex(false, "Local\\NRBGameAdministration"))
            {
                bool owns = false;
                try { owns = mutex.WaitOne(0); }
                catch (System.Threading.AbandonedMutexException) { owns = true; }
                if (!owns) { MessageBox.Show("游戏管理工具已经打开。", "NRB 游戏管理"); return; }
                try
                {
                    Application.EnableVisualStyles();
                    Application.SetCompatibleTextRenderingDefault(false);
                    Application.SetUnhandledExceptionMode(UnhandledExceptionMode.CatchException);
                    Application.ThreadException += delegate(object sender, System.Threading.ThreadExceptionEventArgs e)
                    { Log.Write("Main: UI exception", e.Exception); MessageBox.Show(e.Exception.Message, "操作失败"); };
                    Application.Run(new MainForm(args));
                }
                catch (Exception e) { Log.Write("Main: startup failed", e); MessageBox.Show(e.Message, "启动失败"); }
                finally { if (owns) mutex.ReleaseMutex(); }
            }
        }
    }

    /// <summary>本机日志不记录密码、会话令牌或请求正文。</summary>
    internal static class Log
    {
        internal static readonly string Root = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "NRBGameAdmin");
        internal static void Write(string message, Exception error = null)
        {
            try
            {
                Directory.CreateDirectory(Root);
                string file = Path.Combine(Root, "admin.log");
                if (File.Exists(file) && new FileInfo(file).Length > 1024 * 1024) File.Copy(file, file + ".previous", true);
                if (File.Exists(file) && new FileInfo(file).Length > 1024 * 1024) File.Delete(file);
                File.AppendAllText(file, DateTime.Now.ToString("s") + " " + message + (error == null ? "" : Environment.NewLine + error) + Environment.NewLine, Encoding.UTF8);
            }
            catch { /* 日志目录不可写不阻止用户看到操作结果。 */ }
        }
    }

    /// <summary>只连接部署记录指定的回环接口，不跟随重定向或使用系统代理。</summary>
    internal sealed class Api
    {
        internal string Instance;
        internal int Port;
        internal string Token;
        private static JavaScriptSerializer Serializer() { return new JavaScriptSerializer { MaxJsonLength = 8 * 1024 * 1024 }; }
        internal static string Json(object data) { return Serializer().Serialize(data); }
        internal static Dictionary<string, object> Parse(string data) { return (Dictionary<string, object>)Serializer().DeserializeObject(data); }
        internal static string Text(IDictionary<string, object> data, string key) { return data.ContainsKey(key) && data[key] != null ? Convert.ToString(data[key]) : ""; }
        internal static int Number(IDictionary<string, object> data, string key) { return Convert.ToInt32(data[key]); }
        internal static object[] Array(IDictionary<string, object> data, string key) { return (object[])data[key]; }
        internal static Dictionary<string, object> Obj(params object[] pairs)
        {
            var value = new Dictionary<string, object>();
            for (int i = 0; i < pairs.Length; i += 2) value.Add((string)pairs[i], pairs[i + 1]);
            return value;
        }
        internal Dictionary<string, object> Request(string route, object data = null)
        {
            var request = (HttpWebRequest)WebRequest.Create("http://127.0.0.1:" + Port + "/admin/v1/" + route);
            request.Proxy = null;
            request.AllowAutoRedirect = false;
            request.Method = data == null ? "GET" : "POST";
            request.Timeout = route == "send" || route == "preview" || route == "batch" || route == "cancel" ? 130000 : 8000;
            request.ReadWriteTimeout = request.Timeout;
            request.KeepAlive = false;
            if (!String.IsNullOrEmpty(Token)) request.Headers[HttpRequestHeader.Authorization] = "Bearer " + Token;
            if (data != null)
            {
                request.ContentType = "application/json; charset=utf-8";
                byte[] bytes = Encoding.UTF8.GetBytes(Json(data));
                request.ContentLength = bytes.Length;
                using (Stream output = request.GetRequestStream()) output.Write(bytes, 0, bytes.Length);
            }
            HttpWebResponse response;
            try { response = (HttpWebResponse)request.GetResponse(); }
            catch (WebException e)
            {
                if (e.Response == null) throw new IOException("无法连接服务端，或结果尚未确认。若已点击发送，请查询原批次后再操作。", e);
                using (var failed = (HttpWebResponse)e.Response)
                using (var reader = new StreamReader(failed.GetResponseStream(), Encoding.UTF8))
                {
                    var error = Parse(reader.ReadToEnd());
                    if (failed.StatusCode == HttpStatusCode.Unauthorized) throw new UnauthorizedAccessException(Text(error, "message"));
                    throw new InvalidOperationException(Text(error, "message"));
                }
            }
            using (response)
            using (var reader = new StreamReader(response.GetResponseStream(), Encoding.UTF8))
            {
                var result = Parse(reader.ReadToEnd());
                if (!result.ContainsKey("ok") || !Convert.ToBoolean(result["ok"])) throw new InvalidOperationException(Text(result, "message"));
                return result;
            }
        }
    }

    internal sealed class Item
    {
        public string Key { get; set; }
        public string Kind { get; set; }
        public string Category { get; set; }
        public int Id { get; set; }
        public string Name { get; set; }
        public int Max { get; set; }
        public string Note { get; set; }
    }
    internal sealed class CartItem
    {
        public Item Item { get; set; }
        public string Name { get { return Item.Name; } }
        public int Quantity { get; set; }
    }

    /// <summary>服务状态、登录、目录搜索、传书发放及批次查询窗口。</summary>
    internal sealed class MainForm : Form
    {
        private readonly TextBox directory = new TextBox();
        private readonly TextBox username = new TextBox { Text = "op_root" };
        private readonly TextBox password = new TextBox { UseSystemPasswordChar = true };
        private readonly Label status = new Label { AutoSize = true };
        private readonly Label selection = new Label { AutoSize = true };
        private readonly TextBox result = new TextBox { Dock = DockStyle.Fill, ReadOnly = true,
            Multiline = true, ScrollBars = ScrollBars.Vertical, TabStop = false };
        private readonly Button login = new Button { Text = "登录", Width = 90, Enabled = false };
        private readonly Button browse = new Button { Text = "选择运行目录", Width = 115 };
        private readonly Button logout = new Button { Text = "退出登录", Width = 90, Enabled = false };
        private readonly ComboBox category = new ComboBox { DropDownStyle = ComboBoxStyle.DropDownList, Width = 125 };
        private readonly TextBox search = new TextBox { Width = 225 };
        private readonly NumericUpDown quantity = new NumericUpDown { Minimum = 1, Maximum = 100000000, Value = 1, Width = 110 };
        private readonly Button add = new Button { Text = "加入发放清单", Width = 115 };
        private readonly DataGridView catalogGrid = Grid();
        private readonly DataGridView cartGrid = Grid();
        private readonly BindingList<CartItem> cart = new BindingList<CartItem>();
        private readonly RadioButton single = new RadioButton { Text = "指定游戏名", Checked = true, AutoSize = true };
        private readonly RadioButton all = new RadioButton { Text = "全服现有角色（含离线）", AutoSize = true };
        private readonly TextBox playerName = new TextBox { Width = 200, MaxLength = 64 };
        private readonly Button findPlayer = new Button { Text = "查找角色", Width = 90 };
        private readonly TextBox title = new TextBox { Text = "测试奖励", MaxLength = 40, Dock = DockStyle.Fill };
        private readonly TextBox message = new TextBox { MaxLength = 140, Dock = DockStyle.Fill };
        private readonly Button send = new Button { Text = "预览并发送", Width = 130, Height = 35 };
        private readonly Button query = new Button { Text = "查询待确认批次", Width = 135, Height = 35 };
        private readonly Button cancel = new Button { Text = "取消未完成批次", Width = 135, Height = 35 };
        private readonly Button remove = new Button { Text = "移除选中物品", Width = 120 };
        private readonly Panel workspace = new Panel { Dock = DockStyle.Fill, Enabled = false };
        private readonly DataGridView historyGrid = Grid();
        private readonly System.Windows.Forms.Timer timer = new System.Windows.Forms.Timer { Interval = 5000 };
        private List<Item> items = new List<Item>();
        private Api api;
        private bool busy;
        private bool checking;
        private string pendingId;
        private Dictionary<string, object> pendingPayload;
        private string pendingFile;
        private bool retryAllowed;

        internal MainForm(string[] args)
        {
            Text = "NRB 游戏管理";
            Font = new Font("Microsoft YaHei UI", 9F);
            StartPosition = FormStartPosition.CenterScreen;
            ClientSize = new Size(1180, 870);
            MinimumSize = new Size(1000, 720);
            AutoScaleDimensions = new SizeF(96F, 96F);
            AutoScaleMode = AutoScaleMode.Dpi;
            BuildLayout();
            directory.Text = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "runtime", "server");
            if (args.Length == 2 && args[0] == "--install-dir") directory.Text = Path.GetFullPath(args[1]);
            category.Items.AddRange(new object[] { "全部类别", "货币/经验", "完整卡牌", "卡牌影子", "装备", "奥义", "背包道具", "奥义残章" });
            category.SelectedIndex = 0;
            category.SelectedIndexChanged += delegate { Filter(); };
            search.TextChanged += delegate { Filter(); };
            catalogGrid.SelectionChanged += delegate { SelectItem(); };
            add.Click += delegate { AddItem(); };
            remove.Click += delegate { if (cartGrid.CurrentRow != null) cart.Remove((CartItem)cartGrid.CurrentRow.DataBoundItem); };
            login.Click += async delegate { await Run(Login); };
            logout.Click += async delegate { await Run(Logout); };
            send.Click += async delegate { await Run(Send); };
            query.Click += async delegate { await Run(QueryPending); };
            cancel.Click += async delegate { await Run(CancelPending); };
            findPlayer.Click += async delegate { await Run(FindPlayer); };
            browse.Click += delegate
            {
                using (var dialog = new FolderBrowserDialog { Description = "选择包含 .onepiece-server.json 的服务端运行目录" })
                    if (dialog.ShowDialog(this) == DialogResult.OK) directory.Text = dialog.SelectedPath;
            };
            directory.TextChanged += async delegate { if (!busy) { api = null; await CheckHealth(); } };
            all.CheckedChanged += delegate { playerName.Enabled = !all.Checked; findPlayer.Enabled = !all.Checked; };
            Shown += async delegate
            {
                Rectangle area = Screen.FromControl(this).WorkingArea;
                MinimumSize = new Size(Math.Min(MinimumSize.Width, area.Width - 24), Math.Min(MinimumSize.Height, area.Height - 24));
                Size = new Size(Math.Min(Width, area.Width - 24), Math.Min(Height, area.Height - 24));
                CenterToScreen();
                await CheckHealth(); timer.Start();
            };
            timer.Tick += async delegate { if (!busy) await CheckHealth(); };
            FormClosed += delegate { timer.Stop(); };
            AcceptButton = login;
            UpdateActions();
        }

        private static DataGridView Grid()
        {
            return new DataGridView { Dock = DockStyle.Fill, ReadOnly = true, AutoGenerateColumns = false,
                AllowUserToAddRows = false, AllowUserToDeleteRows = false, AllowUserToResizeRows = false,
                SelectionMode = DataGridViewSelectionMode.FullRowSelect, MultiSelect = false,
                RowHeadersVisible = false, BackgroundColor = Color.White, BorderStyle = BorderStyle.FixedSingle,
                ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.AutoSize };
        }
        private static void Column(DataGridView grid, string property, string caption, int width)
        { grid.Columns.Add(new DataGridViewTextBoxColumn { DataPropertyName = property, HeaderText = caption, Width = width, SortMode = DataGridViewColumnSortMode.NotSortable }); }
        private static FlowLayoutPanel Flow(params Control[] controls)
        {
            var panel = new FlowLayoutPanel { Dock = DockStyle.Top, AutoSize = true,
                AutoSizeMode = AutoSizeMode.GrowAndShrink, WrapContents = true,
                Margin = Padding.Empty, Padding = new Padding(0, 3, 0, 3) };
            panel.Controls.AddRange(controls); return panel;
        }
        private static Label Label(string text) { return new Label { Text = text, AutoSize = true, Margin = new Padding(3, 7, 3, 3) }; }

        private static TableLayoutPanel Field(string text, Control input, Control action = null)
        {
            var row = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true,
                AutoSizeMode = AutoSizeMode.GrowAndShrink, ColumnCount = action == null ? 2 : 3,
                RowCount = 1, Margin = Padding.Empty };
            row.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            row.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            if (action != null) row.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            var caption = Label(text); caption.Anchor = AnchorStyles.Left;
            input.Dock = DockStyle.Fill; input.Margin = new Padding(6, 6, 6, 6);
            row.Controls.Add(caption, 0, 0); row.Controls.Add(input, 1, 0);
            if (action != null) row.Controls.Add(action, 2, 0);
            return row;
        }

        private static void WrapLabel(Label label, Control parent)
        {
            label.AutoSize = true;
            label.Dock = DockStyle.Top;
            parent.SizeChanged += delegate
            {
                int width = Math.Max(100, parent.ClientSize.Width - parent.Padding.Horizontal - label.Margin.Horizontal - 8);
                if (label.MaximumSize.Width != width) label.MaximumSize = new Size(width, 0);
            };
        }

        private void BuildLayout()
        {
            var shell = new TableLayoutPanel { Dock = DockStyle.Fill, Padding = new Padding(15), ColumnCount = 1, RowCount = 4 };
            shell.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            shell.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            shell.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            shell.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            shell.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            foreach (var button in new[] { login, logout, browse, add, remove, findPlayer, send, query, cancel })
            {
                button.MinimumSize = new Size(button.Width, 32);
                button.AutoSize = true; button.AutoSizeMode = AutoSizeMode.GrowAndShrink;
                button.Padding = new Padding(10, 5, 10, 5);
            }
            shell.Controls.Add(new Label { Text = "NRB 游戏管理 · 物品发放", Font = new Font(Font, FontStyle.Bold),
                AutoSize = true, Margin = new Padding(3, 3, 3, 12) }, 0, 0);
            var authPanel = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true,
                AutoSizeMode = AutoSizeMode.GrowAndShrink, ColumnCount = 1, RowCount = 3, Margin = Padding.Empty };
            authPanel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            for (int i = 0; i < 3; i++) authPanel.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            authPanel.Controls.Add(Field("服务运行目录", directory, browse), 0, 0);
            authPanel.Controls.Add(Flow(Label("管理账号"), username, Label("密码"), password, login, logout), 0, 1);
            status.Margin = new Padding(3, 4, 3, 10);
            WrapLabel(status, authPanel); authPanel.Controls.Add(status, 0, 2);
            bool sizingAuth = false;
            Action fitAuth = delegate
            {
                if (sizingAuth || !IsHandleCreated || authPanel.ClientSize.Width == 0) return;
                sizingAuth = true;
                try
                {
                    int height = 0;
                    foreach (Control child in authPanel.Controls)
                        height += child.GetPreferredSize(new Size(Math.Max(100, authPanel.ClientSize.Width - child.Margin.Horizontal), 0)).Height + child.Margin.Vertical;
                    var maximum = new Size(0, height);
                    if (authPanel.MaximumSize != maximum) authPanel.MaximumSize = maximum;
                }
                finally { sizingAuth = false; }
            };
            authPanel.Layout += delegate { fitAuth(); };
            Shown += delegate { fitAuth(); };
            shell.Controls.Add(authPanel, 0, 1);
            workspace.AutoScroll = true;
            workspace.AutoScrollMinSize = new Size(900, 400);
            // 滚动内容不能 Dock=Fill，否则布局会把滚动后的位置重新归零。
            var tabs = new TabControl { Dock = DockStyle.None, Anchor = AnchorStyles.Top | AnchorStyles.Left,
                MinimumSize = new Size(900, 400) };
            var grantTab = new TabPage("发放物品");
            var historyTab = new TabPage("发放记录");
            tabs.TabPages.Add(grantTab); tabs.TabPages.Add(historyTab);
            var split = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 2, RowCount = 1, Padding = new Padding(4) };
            split.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 53)); split.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 47));
            split.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            var left = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 4 };
            left.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            left.RowStyles.Add(new RowStyle(SizeType.AutoSize)); left.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            left.RowStyles.Add(new RowStyle(SizeType.AutoSize)); left.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            var filter = Field("名称 / ID", search);
            filter.ColumnCount = 3;
            filter.ColumnStyles.Insert(0, new ColumnStyle(SizeType.AutoSize));
            filter.Controls.Clear();
            filter.Controls.Add(category, 0, 0); filter.Controls.Add(Label("名称 / ID"), 1, 0); filter.Controls.Add(search, 2, 0);
            category.Margin = new Padding(3, 6, 8, 6);
            left.Controls.Add(filter, 0, 0);
            Column(catalogGrid, "Category", "类别", 80); Column(catalogGrid, "Id", "模板 ID", 80); Column(catalogGrid, "Name", "名称", 280);
            catalogGrid.Columns[2].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
            catalogGrid.Columns[0].AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
            catalogGrid.Columns[1].AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
            catalogGrid.Columns[2].DefaultCellStyle.WrapMode = DataGridViewTriState.True;
            catalogGrid.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.DisplayedCells;
            catalogGrid.RowTemplate.Height = Math.Max(28, Font.Height + 10);
            left.Controls.Add(catalogGrid, 0, 1); left.Controls.Add(selection, 0, 2);
            selection.Margin = new Padding(4, 8, 4, 8); WrapLabel(selection, left);
            left.Controls.Add(Flow(Label("数量"), quantity, add), 0, 3);
            var right = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 9, Padding = new Padding(10, 0, 0, 0) };
            right.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            for (int row = 0; row < 9; row++) right.RowStyles.Add(new RowStyle(row == 3 ? SizeType.Percent : SizeType.AutoSize, row == 3 ? 100 : 0));
            right.Controls.Add(Flow(single, all), 0, 0);
            right.Controls.Add(Field("角色名", playerName, findPlayer), 0, 1);
            right.Controls.Add(Label("发放清单（每批最多 20 种物品）"), 0, 2);
            Column(cartGrid, "Name", "物品", 290); Column(cartGrid, "Quantity", "数量", 90);
            cartGrid.Columns[0].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
            cartGrid.Columns[0].DefaultCellStyle.WrapMode = DataGridViewTriState.True;
            cartGrid.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.AllCells;
            cartGrid.RowTemplate.Height = Math.Max(28, Font.Height + 10);
            cartGrid.MinimumSize = new Size(0, 100);
            cartGrid.DataSource = cart; right.Controls.Add(cartGrid, 0, 3);
            right.Controls.Add(Flow(remove), 0, 4);
            right.Controls.Add(Field("标题", title), 0, 5);
            right.Controls.Add(Field("说明", message), 0, 6);
            var instructions = Label("在游戏内“传书 → 系统”领取；已有完整卡牌会转为影子。");
            WrapLabel(instructions, right); right.Controls.Add(instructions, 0, 7);
            right.Controls.Add(Flow(send, query, cancel), 0, 8);
            split.Controls.Add(left, 0, 0); split.Controls.Add(right, 1, 0); grantTab.Controls.Add(split);
            var historyLayout = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 2 };
            historyLayout.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            historyLayout.RowStyles.Add(new RowStyle(SizeType.AutoSize)); historyLayout.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            var refresh = new Button { Text = "刷新最近 50 批", AutoSize = true, Padding = new Padding(10, 5, 10, 5) };
            refresh.Click += async delegate { await Run(History); };
            historyLayout.Controls.Add(Flow(refresh, Label("记录表示传书已入库；玩家需在游戏内领取。")), 0, 0);
            historyGrid.AutoGenerateColumns = true; historyLayout.Controls.Add(historyGrid, 0, 1); historyTab.Controls.Add(historyLayout);
            workspace.Controls.Add(tabs); shell.Controls.Add(workspace, 0, 2);
            bool measuring = false;
            Action fitWorkspace = delegate
            {
                if (measuring || !IsHandleCreated) return;
                measuring = true;
                try
                {
                    int reserved = 0;
                    int[] rows = right.GetRowHeights();
                    for (int i = 0; i < rows.Length; i++) if (i != 3) reserved += rows[i];
                    // 小窗口通过滚动展示完整表单，不能把发放清单压成几像素。
                    var minimum = new Size(900, Math.Max(430, reserved + cartGrid.MinimumSize.Height + 44));
                    if (tabs.MinimumSize != minimum) tabs.MinimumSize = minimum;
                    if (workspace.AutoScrollMinSize != minimum) workspace.AutoScrollMinSize = minimum;
                    var contentSize = new Size(
                        Math.Max(minimum.Width, workspace.ClientSize.Width - (workspace.VerticalScroll.Visible ? SystemInformation.VerticalScrollBarWidth : 0)),
                        Math.Max(minimum.Height, workspace.ClientSize.Height - (workspace.HorizontalScroll.Visible ? SystemInformation.HorizontalScrollBarHeight : 0)));
                    if (tabs.Size != contentSize) tabs.Size = contentSize;
                }
                finally { measuring = false; }
            };
            right.Layout += delegate { fitWorkspace(); };
            workspace.ClientSizeChanged += delegate { fitWorkspace(); };
            Shown += delegate { fitWorkspace(); };
            result.Height = Math.Max(64, Font.Height * 3 + 14);
            result.Margin = new Padding(3, 8, 3, 3);
            result.Text = "先启动服务端，再登录管理账号。"; shell.Controls.Add(result, 0, 3);
            Controls.Add(shell);
        }

        private async Task Run(Func<Task> action)
        {
            if (busy) return;
            busy = true; workspace.Enabled = false; login.Enabled = logout.Enabled = browse.Enabled = directory.Enabled = false;
            UseWaitCursor = true;
            try { await action(); }
            catch (UnauthorizedAccessException e)
            { if (api != null) api.Token = null; status.Text = "请重新登录"; Log.Write("Run: authorization rejected"); MessageBox.Show(this, e.Message, "登录状态"); }
            catch (Exception e)
            { Log.Write("Run: operation failed", e); result.Text = e.Message; MessageBox.Show(this, e.Message, "操作未完成", MessageBoxButtons.OK, MessageBoxIcon.Warning); }
            finally { busy = false; UseWaitCursor = false; UpdateActions(); }
            await CheckHealth();
        }

        private async Task CheckHealth()
        {
            if (checking || busy) return;
            checking = true;
            string path = directory.Text;
            try
            {
                var current = await Task.Run(delegate
                {
                    string marker = Path.Combine(Path.GetFullPath(path), ".onepiece-server.json");
                    if (!File.Exists(marker)) throw new IOException("未找到部署记录，请先启动服务端，或选择正确运行目录。" );
                    var state = Api.Parse(File.ReadAllText(marker, Encoding.UTF8));
                    if (!state.ContainsKey("installed") || !Convert.ToBoolean(state["installed"])) throw new IOException("服务端部署尚未完成。" );
                    if (!state.ContainsKey("adminPort") || !state.ContainsKey("adminInstanceId")) throw new IOException("管理接口尚未配置：更新仓库后，请停止服务并重新启动。" );
                    var candidate = new Api { Port = Api.Number(state, "adminPort"), Instance = Api.Text(state, "adminInstanceId") };
                    if (candidate.Port < 1024 || candidate.Port > 65535 || candidate.Instance.Length != 32) throw new IOException("管理接口配置无效。" );
                    var health = candidate.Request("health");
                    if (Api.Text(health, "instanceId") != candidate.Instance || Api.Number(health, "protocol") != 1) throw new IOException("接口与当前部署不匹配。" );
                    return candidate;
                });
                if (path != directory.Text || busy) return;
                if (api == null || api.Instance != current.Instance || api.Port != current.Port) api = current;
                status.Text = "服务端就绪 · " + (String.IsNullOrEmpty(api.Token) ? "可以登录" : "已登录");
                status.ForeColor = Color.SeaGreen;
                login.Enabled = String.IsNullOrEmpty(api.Token);
                UpdateActions();
            }
            catch (Exception e)
            {
                if (busy || path != directory.Text) return;
                login.Enabled = false; workspace.Enabled = false;
                status.ForeColor = Color.Firebrick; status.Text = "服务端不可用：" + e.Message;
                if (api != null) api.Token = null;
                UpdateActions();
                login.Enabled = false;
            }
            finally { checking = false; }
        }

        private async Task Login()
        {
            if (api == null) throw new InvalidOperationException("请先启动服务端。");
            var active = api;
            var payload = Api.Obj("username", username.Text.Trim(), "password", password.Text);
            var reply = await Task.Run(() => active.Request("login", payload));
            active.Token = Api.Text(reply, "token"); password.Clear();
            var data = await Task.Run(() => active.Request("catalog", Api.Obj()));
            items = Api.Array(data, "items").Cast<Dictionary<string, object>>().Select(x => new Item {
                Key = Api.Text(x, "key"), Kind = Api.Text(x, "kind"), Category = Category(Api.Text(x, "kind")),
                Id = Api.Number(x, "id"), Name = Api.Text(x, "name"), Max = Api.Number(x, "max"), Note = Api.Text(x, "note") }).ToList();
            pendingFile = Path.Combine(Log.Root, "pending-" + api.Instance + ".json");
            pendingId = null; pendingPayload = null; retryAllowed = false;
            if (File.Exists(pendingFile))
            {
                var pending = Api.Parse(File.ReadAllText(pendingFile, Encoding.UTF8));
                pendingId = Api.Text(pending, "batchId");
                pendingPayload = (Dictionary<string, object>)pending["payload"];
                result.Text = "有待确认批次 " + pendingId + "，请先查询结果。";
            }
            else result.Text = "登录成功，已加载 " + items.Count + " 个可发放条目。";
            Filter(); AcceptButton = null;
        }

        private async Task Logout()
        {
            try { if (api != null && api.Token != null) await Task.Run(() => api.Request("logout", Api.Obj())); }
            finally { if (api != null) api.Token = null; password.Clear(); cart.Clear(); items.Clear(); Filter(); }
        }

        private static string Category(string kind)
        {
            switch (kind) { case "property": return "货币/经验"; case "card": return "完整卡牌"; case "soul": return "卡牌影子";
                case "equip": return "装备"; case "skill": return "奥义"; case "tool": return "背包道具"; case "piece": return "奥义残章"; default: return kind; }
        }
        private void Filter()
        {
            string text = search.Text.Trim(); string type = Convert.ToString(category.SelectedItem);
            catalogGrid.DataSource = new BindingList<Item>(items.Where(x => (category.SelectedIndex <= 0 || x.Category == type)
                && (text.Length == 0 || x.Name.IndexOf(text, StringComparison.OrdinalIgnoreCase) >= 0 || x.Id.ToString().Contains(text))).ToList());
        }
        private void SelectItem()
        {
            var item = catalogGrid.CurrentRow == null ? null : catalogGrid.CurrentRow.DataBoundItem as Item;
            if (item == null) { selection.Text = "请选择物品"; return; }
            quantity.Maximum = item.Max;
            selection.Text = item.Name + " · 单批最多 " + item.Max + (item.Note.Length == 0 ? "" : Environment.NewLine + item.Note);
        }
        private void AddItem()
        {
            var item = catalogGrid.CurrentRow == null ? null : catalogGrid.CurrentRow.DataBoundItem as Item;
            if (item == null) return;
            var existing = cart.FirstOrDefault(x => x.Item.Key == item.Key);
            if (existing != null) { existing.Quantity = (int)quantity.Value; cart.ResetBindings(); return; }
            if (cart.Count == 20) { MessageBox.Show(this, "每批最多选择 20 种物品。"); return; }
            cart.Add(new CartItem { Item = item, Quantity = (int)quantity.Value });
        }

        private void UpdateActions()
        {
            bool loggedIn = api != null && !String.IsNullOrEmpty(api.Token);
            workspace.Enabled = loggedIn && !busy;
            directory.Enabled = browse.Enabled = !loggedIn && !busy;
            username.Enabled = password.Enabled = !loggedIn && !busy;
            logout.Enabled = loggedIn && !busy;
            bool pending = !String.IsNullOrEmpty(pendingId);
            add.Enabled = remove.Enabled = single.Enabled = all.Enabled = title.Enabled = message.Enabled = !pending;
            playerName.Enabled = !all.Checked && !pending; findPlayer.Enabled = !all.Checked && !pending;
            send.Text = pending ? "预览并重试原批次" : "预览并发送";
            send.Enabled = !pending || retryAllowed;
            query.Enabled = cancel.Enabled = pending;
        }

        private async Task FindPlayer()
        {
            var response = await Task.Run(() => api.Request("players", Api.Obj("name", playerName.Text.Trim())));
            var list = Api.Array(response, "players").Cast<Dictionary<string, object>>().Select(x => new {
                角色ID = Api.Number(x, "id"), 游戏名 = Api.Text(x, "name"), 等级 = Api.Number(x, "level") }).ToList();
            if (list.Count == 0) { MessageBox.Show(this, "未找到角色。"); return; }
            using (var dialog = new Form { Text = "选择角色（最多显示 100 项）", Size = new Size(560, 420), StartPosition = FormStartPosition.CenterParent })
            {
                var grid = Grid(); grid.AutoGenerateColumns = true; grid.DataSource = list;
                var choose = new Button { Text = "使用选中游戏名", Dock = DockStyle.Bottom, Height = 36 };
                choose.Click += delegate { if (grid.CurrentRow != null) { playerName.Text = list[grid.CurrentRow.Index].游戏名; dialog.Close(); } };
                dialog.Controls.Add(grid); dialog.Controls.Add(choose); dialog.ShowDialog(this);
            }
        }

        private async Task Send()
        {
            if (pendingId != null && !retryAllowed) throw new InvalidOperationException("请先查询待确认批次。");
            var payload = pendingId == null ? Api.Obj("all", all.Checked, "name", playerName.Text.Trim(), "title", title.Text,
                "message", message.Text, "items", cart.Select(x => Api.Obj("key", x.Item.Key, "quantity", x.Quantity)).ToArray()) : pendingPayload;
            var preview = await Task.Run(() => api.Request("preview", payload));
            string itemList = String.Join(Environment.NewLine, Api.Array(payload, "items").Cast<Dictionary<string, object>>().Select(x => {
                var item = items.FirstOrDefault(i => i.Key == Api.Text(x, "key"));
                return (item == null ? Api.Text(x, "key") : item.Name) + " × " + Api.Number(x, "quantity"); }));
            string description = "接收范围：" + Api.Text(preview, "target") + Environment.NewLine + "角色数：" + Api.Number(preview, "players")
                + "；每人传书：" + Api.Number(preview, "mailsPerPlayer") + Environment.NewLine + Environment.NewLine + itemList;
            if (MessageBox.Show(this, description + Environment.NewLine + Environment.NewLine + "确认发放以上物品？", "确认发放", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) != DialogResult.OK) return;
            if (pendingId == null) { pendingId = Guid.NewGuid().ToString("N"); pendingPayload = payload; }
            SavePending(); retryAllowed = false;
            result.Text = "正在提交批次：" + pendingId;
            Log.Write("Send: submitting batchId=" + pendingId);
            var receipt = await Task.Run(() => api.Request("send", Api.Obj("ticket", Api.Text(preview, "ticket"), "batchId", pendingId)));
            Complete(receipt);
        }

        private void SavePending()
        {
            Directory.CreateDirectory(Log.Root);
            string temporary = pendingFile + ".writing";
            File.WriteAllText(temporary, Api.Json(Api.Obj("batchId", pendingId, "payload", pendingPayload)), new UTF8Encoding(false));
            if (File.Exists(pendingFile)) File.Replace(temporary, pendingFile, null); else File.Move(temporary, pendingFile);
        }
        private async Task QueryPending()
        {
            if (pendingId == null) return;
            var receipt = await Task.Run(() => api.Request("batch", Api.Obj("batchId", pendingId)));
            if (Convert.ToBoolean(receipt["found"])) Complete(receipt);
            else { retryAllowed = true; result.Text = "当前未查到已提交记录，可按原批次重试，或取消未完成批次。批次：" + pendingId; }
        }
        private async Task CancelPending()
        {
            if (pendingId == null) return;
            var receipt = await Task.Run(() => api.Request("cancel", Api.Obj("batchId", pendingId)));
            Complete(receipt);
        }
        private void Complete(Dictionary<string, object> receipt)
        {
            bool canceled = receipt.ContainsKey("canceled") && Convert.ToBoolean(receipt["canceled"]);
            string description = canceled ? "未完成批次已取消。" : "传书已入库：" + Api.Number(receipt, "players") + " 个角色，" + Api.Number(receipt, "mails") + " 封传书。请到游戏内领取。";
            result.Text = description + " 批次：" + Api.Text(receipt, "batchId");
            Log.Write("Complete: batchId=" + Api.Text(receipt, "batchId") + ", canceled=" + canceled);
            if (File.Exists(pendingFile)) File.Delete(pendingFile);
            pendingId = null; pendingPayload = null; retryAllowed = false; cart.Clear();
            MessageBox.Show(this, result.Text, "批次结果", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
        private async Task History()
        {
            var response = await Task.Run(() => api.Request("history", Api.Obj()));
            historyGrid.DataSource = Api.Array(response, "batches").Cast<Dictionary<string, object>>().Select(x => new {
                批次号 = Api.Text(x, "batchId"), 状态 = x.ContainsKey("canceled") && Convert.ToBoolean(x["canceled"]) ? "已取消" : "传书已入库",
                角色数 = Api.Number(x, "players"), 传书数 = Api.Number(x, "mails"), 范围 = Api.Text(x, "target"),
                时间 = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc).AddSeconds(Api.Number(x, "time")).ToLocalTime().ToString("yyyy-MM-dd HH:mm:ss") }).ToList();
            historyGrid.AutoResizeColumns(DataGridViewAutoSizeColumnsMode.AllCells);
        }
    }
}
