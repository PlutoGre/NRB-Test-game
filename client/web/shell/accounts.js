'use strict';
// All identities and tokens come from the shared Java service. Browser storage
// is only a convenience; it never determines which server character is loaded.
(() => {
  const $ = id => document.getElementById(id);
  let current = null, storageKey = '', pending = null, rememberedName = '';
  async function request(action, body) {
    const controller = new AbortController(), timer = setTimeout(() => controller.abort(), 20000);
    try {
      const response = await fetch('/account/v1/' + action, {method: 'POST', credentials: 'same-origin', cache: 'no-store',
        headers: {'Content-Type': 'application/json'}, body: JSON.stringify(body), signal: controller.signal});
      const result = await response.json();
      if (!response.ok || !result.ok) {
        const error = new Error(result.message || '账号验证失败'); error.status = response.status; throw error;
      }
      return result;
    } finally { clearTimeout(timer); }
  }
  function saved() {
    try { return JSON.parse(localStorage.getItem(storageKey) || 'null'); } catch { return null; }
  }
  function accept(value) {
    if (!/^acct_[a-f0-9]{32}$/.test(value.uid) || !/^nrb1_[a-f0-9]{64}$/.test(value.token)) throw new Error('请更新服务端账号接口');
    current = value; rememberedName = value.username;
    Object.assign(window.NRB, {identity: value.uid, accountToken: value.token, accountName: value.username});
    try { localStorage.setItem(storageKey, JSON.stringify({username: value.username, token: value.token})); }
    catch { window.NRB.message('浏览器禁止本地存储，下次打开需要重新输入账号密码。角色进度仍保存在服务端。'); }
    $('switch-account').hidden = false; $('status').textContent = '已登录：' + value.username;
    return value;
  }
  function form(message = '') {
    return new Promise(resolve => {
      const dialog = $('account'), form = $('account-form'); let register = false, busy = false;
      const name = $('account-name'), password = $('account-password'), confirm = $('account-confirm'), error = $('account-error');
      name.value = rememberedName || saved()?.username || ''; password.value = ''; confirm.value = ''; error.textContent = message;
      const mode = () => {
        $('account-title').textContent = register ? '注册游戏账号' : '登录游戏账号';
        $('account-submit').textContent = register ? '注册并登录' : '登录';
        $('account-toggle').textContent = register ? '已有账号' : '注册账号';
        $('account-confirm-row').hidden = !register; confirm.required = register;
        password.autocomplete = register ? 'new-password' : 'current-password';
      };
      const finish = value => { dialog.close(); password.value = ''; confirm.value = ''; form.onsubmit = null; resolve(value); };
      dialog.oncancel = event => { event.preventDefault(); if (!busy) finish(null); };
      $('account-cancel').onclick = () => { if (!busy) finish(null); };
      $('account-toggle').onclick = () => { if (!busy) { register = !register; password.value = ''; confirm.value = ''; error.textContent = ''; mode(); } };
      form.onsubmit = async event => {
        event.preventDefault(); if (busy) return;
        if (register && password.value !== confirm.value) { error.textContent = '两次输入的密码不一致'; return; }
        busy = true; form.querySelectorAll('button').forEach(button => button.disabled = true); error.textContent = '正在连接账号服务…';
        try {
          const body = {username: name.value.trim(), password: password.value}; if (register) body.confirmPassword = confirm.value;
          finish(accept(await request(register ? 'register' : 'login', body)));
        } catch (failure) { error.textContent = failure.name === 'AbortError' ? '连接超时，请确认服务端已启动后重试' : failure.message; }
        finally { busy = false; form.querySelectorAll('button').forEach(button => button.disabled = false); }
      };
      mode(); dialog.showModal(); name.focus();
    });
  }
  async function forget() {
    const previous = current || saved(); rememberedName = previous?.username || rememberedName;
    current = null; Object.assign(window.NRB, {identity: '', accountToken: '', accountName: ''});
    try { localStorage.removeItem(storageKey); } catch { /* Continue without browser storage. */ }
    $('switch-account').hidden = true;
    if (previous?.token) {
      // Wait before issuing a new cookie: a late logout response must not clear
      // the freshly authenticated account's transport cookie.
      try { await request('logout', {token: previous.token}); }
      catch { /* Discarded local tokens are never used as a guest fallback. */ }
    }
  }
  function ensure() {
    if (current) return Promise.resolve(current);
    if (pending) return pending;
    pending = (async () => {
      let errorMessage = ''; const value = saved();
      if (value?.token) {
        try { return accept(await request('resume', {token: value.token})); }
        catch (error) {
          errorMessage = error.status === 401 ? '登录已过期，请重新输入账号密码' : '自动登录失败，请检查服务端连接或手动登录';
          if (error.status === 401) {
            try { localStorage.setItem(storageKey, JSON.stringify({username: value.username})); } catch { /* Storage optional. */ }
          }
        }
      }
      return form(errorMessage);
    })().finally(() => { pending = null; });
    return pending;
  }
  window.NRBAccounts = {configure(instance) { storageKey = 'nrb.account.v1.' + instance; }, ensure, forget};
})();
