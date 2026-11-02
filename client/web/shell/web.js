'use strict';
// This shell hosts the original Cocos scene graph. It contains no duplicate
// gameplay, item logic or simulated login screens.
(() => {
  const $ = id => document.getElementById(id);
  const status = $('status'), start = $('start'), canvas = $('game');
  let gameModule, flushTimer, boardTimer, boardGeneration = 0, messageQueue = [];
  const nativeString = (module, name, value, extra = []) => module.ccall(name, null,
    ['string', ...extra.map(() => 'number')], [value, ...extra]);
  function fatal(error) {
    $('boot').hidden = false; $('progress').hidden = true; $('waiting').hidden = true;
    start.disabled = false; start.textContent = '刷新重试'; start.onclick = () => location.reload();
    status.textContent = '游戏未能启动'; $('error').hidden = false;
    $('error').textContent = String(error?.message || error) + '\n请查看浏览器控制台和服务端 runtime/server/logs/browser.stderr.log。';
    console.error('[NRB Web]', error);
  }
  function message(text, done) {
    messageQueue.push({text, done});
    if (messageQueue.length === 1) showMessage();
  }
  function showMessage() {
    const dialog = $('message'); dialog.querySelector('p').textContent = messageQueue[0].text;
    if (!dialog.open) dialog.showModal();
  }
  $('message').querySelector('button').onclick = () => $('message').close();
  $('message').addEventListener('close', () => {
    const entry = messageQueue.shift(); if (entry?.done) entry.done();
    if (messageQueue.length) showMessage();
  });
  function editControl({value = '', placeholder = '', maximum = -1, password = false, multiline = false, mode = 6, change, finish}) {
    const previous = $('input');
    if (previous.open) previous.close('cancel');
    // A close event is queued asynchronously. Give each editor its own dialog so
    // a late close from the old control cannot commit the new control's text.
    const dialog = previous.cloneNode(true); previous.replaceWith(dialog);
    const field = document.createElement(multiline ? 'textarea' : 'input');
    if (!multiline) field.type = password ? 'password' : mode === 1 ? 'email' : mode === 3 ? 'tel' : 'text';
    if (mode === 2 || mode === 5) field.inputMode = mode === 2 ? 'numeric' : 'decimal';
    field.value = value; field.placeholder = placeholder; field.autocomplete = 'off';
    if (maximum >= 0) field.maxLength = maximum;
    field.addEventListener('input', () => { if (!field.composing && change) change(field.value); });
    field.addEventListener('compositionstart', () => { field.composing = true; });
    field.addEventListener('compositionend', () => { field.composing = false; if (change) change(field.value); });
    $('input-control').replaceChildren(field);
    dialog.onclose = () => {
      const accepted = dialog.returnValue !== 'cancel'; dialog.onclose = null;
      if ($('input') === dialog) NRB.editing = 0;
      if (finish) finish(field.value, accepted);
      if ($('input') === dialog) canvas.focus();
    };
    dialog.returnValue = ''; dialog.showModal(); field.focus();
  }
  const audio = {
    context: null, cache: new Map(), entries: new Map(),
    unlock() {
      const Constructor = window.AudioContext || window.webkitAudioContext;
      if (!Constructor) return;
      this.context ||= new Constructor(); this.context.resume().catch(error => console.warn('[NRB Audio]', error));
    },
    load(module, path) {
      if (!this.context) return Promise.resolve(null);
      if (!this.cache.has(path)) {
        const promise = Promise.resolve().then(() => {
          const bytes = module.FS.readFile(path); return this.context.decodeAudioData(bytes.slice().buffer);
        }).catch(error => { this.cache.delete(path); console.warn('[NRB Audio] Unable to decode', path, error); return null; });
        this.cache.set(path, promise);
      }
      return this.cache.get(path);
    },
    play(module, path, id, loop, volume) {
      this.stop(id);
      const entry = {module, path, id, loop, volume, offset: 0, paused: false, source: null};
      this.entries.set(id, entry);
      this.load(module, path).then(buffer => {
        if (buffer && this.entries.get(id) === entry) { entry.buffer = buffer; if (!entry.paused) this.resume(id); }
      });
    },
    resume(id) {
      const entry = this.entries.get(id); if (!entry || !entry.buffer || entry.source || !this.context) return;
      const source = this.context.createBufferSource(), gain = this.context.createGain();
      source.buffer = entry.buffer; source.loop = entry.loop; gain.gain.value = entry.volume;
      source.connect(gain).connect(this.context.destination);
      entry.source = source; entry.gain = gain; entry.paused = false; entry.started = this.context.currentTime;
      const offset = entry.loop ? entry.offset % entry.buffer.duration : Math.min(entry.offset, Math.max(0, entry.buffer.duration - .001));
      source.onended = () => { if (entry.source === source && !entry.paused) this.stop(id); };
      source.start(0, offset);
    },
    pause(id) {
      const entry = this.entries.get(id); if (!entry) return; entry.paused = true;
      if (entry.source) {
        entry.offset += this.context.currentTime - entry.started; entry.source.onended = null;
        entry.source.stop(); entry.source.disconnect(); entry.gain.disconnect(); entry.source = null;
      }
    },
    stop(id, release = false) { const entry = this.entries.get(id); this.pause(id); this.entries.delete(id); if (release && entry) this.cache.delete(entry.path); },
    rewind(id) { const entry = this.entries.get(id); if (entry) { this.pause(id); entry.offset = 0; this.resume(id); } },
    playing(id) { const entry = this.entries.get(id); return !!(entry?.source && !entry.paused); },
    each(method) { for (const id of [...this.entries.keys()]) if (id !== 0) this[method](id); },
    volume(music, value) { for (const [id, entry] of this.entries) if ((id === 0) === music) { entry.volume = value; if (entry.gain) entry.gain.gain.value = value; } }
  };
  const NRB = window.NRB = {
    audio, editing: 0, identity: '', message,
    waiting(value) { $('waiting').hidden = !value; },
    closeInput() { if ($('input').open) $('input').close('cancel'); },
    input(module, multiline) {
      editControl({multiline: !!multiline, finish: (value, accepted) => { if (accepted) nativeString(module, 'nrb_input', value); }});
    },
    edit(module, id, value, placeholder, maximum, password, mode) {
      this.editing = id;
      const send = (text, finished) => module.ccall('nrb_edit', null, ['number', 'string', 'number'], [id, text, finished]);
      editControl({value, placeholder, maximum, password: !!password, multiline: mode === 0, mode,
        change: text => send(text, 0), finish: text => send(text, 1)});
      this.editing = id;
    },
    ime(module, value) {
      editControl({value, finish: (text, accepted) => { if (accepted) nativeString(module, 'nrb_ime', text); }});
    },
    openURL(value) {
      try { const url = new URL(value, location.origin); if (['http:', 'https:'].includes(url.protocol)) window.open(url.href, '_blank', 'noopener,noreferrer'); }
      catch (error) { console.warn('[NRB URL]', error); }
    },
    closeBoard() {
      ++boardGeneration; clearTimeout(boardTimer);
      const dialog = $('board'); dialog.onclose = null;
      dialog.querySelector('iframe').onload = null;
      if (dialog.open) dialog.close(); dialog.querySelector('iframe').src = 'about:blank';
    },
    board(module, value, seconds) {
      this.closeBoard(); const generation = boardGeneration;
      const previous = $('board'), dialog = previous.cloneNode(true); previous.replaceWith(dialog);
      const frame = dialog.querySelector('iframe');
      const done = event => { if (generation === boardGeneration) module._nrb_board(event); };
      let url;
      try { url = new URL(value, location.origin); } catch { done(3); return; }
      // Legacy announcement hosts may be offline or contain obsolete scripts.
      // Keep an explicit close action and never grant them native game access.
      if (!['http:', 'https:'].includes(url.protocol) || (location.protocol === 'https:' && url.protocol !== 'https:')) { done(3); return; }
      frame.onload = () => { clearTimeout(boardTimer); done(0); };
      dialog.onclose = () => done(1);
      $('close-board').onclick = () => dialog.close();
      dialog.showModal(); frame.src = url.href; boardTimer = setTimeout(() => done(2), Math.max(1, seconds) * 1000);
    }
  };
  function flush() {
    if (!gameModule || flushTimer) return;
    flushTimer = true;
    gameModule.FS.syncfs(false, error => {
      flushTimer = false;
      if (error) console.warn('[NRB Storage] Could not persist browser cache', error);
    });
  }
  canvas.addEventListener('contextmenu', event => event.preventDefault());
  canvas.addEventListener('webglcontextlost', event => { event.preventDefault(); fatal('浏览器图形上下文已丢失，请刷新页面重新登录。'); });
  let backgroundSounds = [];
  canvas.addEventListener('pointerdown', () => audio.unlock(), {passive: true});
  document.addEventListener('visibilitychange', () => {
    if (document.hidden) {
      flush(); backgroundSounds = [...audio.entries.keys()].filter(id => audio.playing(id));
      backgroundSounds.forEach(id => audio.pause(id));
    } else {
      backgroundSounds.forEach(id => audio.resume(id)); backgroundSounds = [];
    }
    if (gameModule) gameModule._nrb_visibility(document.hidden ? 1 : 0);
  });
  window.addEventListener('pagehide', flush);
  window.addEventListener('unhandledrejection', event => { if (!gameModule) fatal(event.reason); });
  setInterval(flush, 5000);

  async function request(path, options) {
    const response = await fetch(path, {credentials: 'same-origin', cache: 'no-store', ...options});
    const value = await response.json();
    if (!response.ok) throw new Error(value.error || ('HTTP ' + response.status));
    return value;
  }
  start.onclick = async () => {
    start.disabled = true; audio.unlock(); status.textContent = '正在建立浏览器身份…';
    try {
      const identity = await request('/web/session', {method: 'POST'}); NRB.identity = identity.uid;
      status.textContent = '正在下载原版游戏和资源…';
      await new Promise((resolve, reject) => {
        const script = document.createElement('script'); script.src = '/game.js'; script.onload = resolve;
        script.onerror = () => reject(new Error('无法加载 game.js，请重新构建网页客户端。')); document.head.append(script);
      });
      const module = await createNRBGame({
        canvas, print: (...args) => console.log('[NRB Game]', ...args), printErr: (...args) => console.error('[NRB Game]', ...args),
        locateFile: name => '/' + name,
        setStatus(text) {
          if (text) status.textContent = text;
          const progress = String(text).match(/\((\d+(?:\.\d+)?)\/(\d+)\)/);
          if (progress) $('progress').value = Number(progress[1]) / Number(progress[2]) * 100;
        },
        monitorRunDependencies(remaining) { if (remaining) status.textContent = '正在载入游戏资源…（' + remaining + '）'; },
        onAbort: reason => fatal('WebAssembly 运行失败：' + reason),
        preRun: [module => {
          module.addRunDependency('nrb-persistent-files');
          module.FS.mkdirTree('/persist'); module.FS.mount(module.IDBFS, {}, '/persist');
          module.FS.syncfs(true, error => {
            if (error) console.warn('[NRB Storage] Browser cache unavailable; server character is retained', error);
            module.removeRunDependency('nrb-persistent-files');
          });
        }],
        onRuntimeInitialized() { $('boot').hidden = true; canvas.focus(); }
      });
      gameModule = module;
    } catch (error) { fatal(error); }
  };
  if (!window.WebAssembly || !window.WebSocket) { fatal('请使用支持 WebAssembly、WebGL 和 WebSocket 的浏览器。'); return; }
  request('/web/config').then(config => {
    status.textContent = config.serverName + '已就绪'; start.disabled = false;
  }).catch(fatal);
})();
