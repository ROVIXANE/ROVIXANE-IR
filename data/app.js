/* IHR Universal IR Remote - offline dashboard. No external dependencies. */
(function () {
  'use strict';

  var ICONS = ['⏻','📺','🔊','🔈','🔇','➕','➖','⬆','⬇','⬅','➡','✅',
               '↩','🏠','☰','▶','⏸','⏹','⏮','⏭','❄','💡','🌀','⭐'];
  var COLORS = ['#4f8cff','#ff6b6b','#37d67a','#ffc857','#b06dff','#00c2c7','#e8eefc'];
  var PROTOCOLS = ['NEC','LG','LG2','SONY','SAMSUNG','JVC','PANASONIC',
                   'RC5','RC6','DENON','COOLIX','MIDEA24'];

  var STR = {
    en: {
      appName:'IHR IR Remote', connecting:'Connecting…', online:'Online', apMode:'AP mode',
      tabRemotes:'Remotes', tabLearn:'Learn', tabSettings:'Settings', tabDiag:'Diagnostics',
      addProfile:'+ Profile', edit:'Edit', duplicate:'Duplicate', delete:'Delete',
      emptyHint:'No buttons yet. Add one or learn from your remote.', addSignal:'+ Add button',
      learnTitle:'Learn a signal', save:'Save', cancel:'Cancel', startLearning:'Start learning',
      learning:'Learning… press a button on your remote', saveAsButton:'Save as button',
      learnHelp:'Point your remote at the TSOP38238 receiver and press a button. Protocol, HEX value, bit length and raw timings are shown below.',
      learnIdle:'No capture yet. Start learning, then press a remote button.',
      settingsTitle:'Settings', deviceNameLbl:'Device name', apSsidLbl:'Config AP SSID',
      apPassLbl:'Config AP password (empty = open AP)', wifiSsidLbl:'WiFi network (SSID)',
      wifiPassLbl:'WiFi password', captiveLbl:'Captive portal', httpAuthLbl:'Require login',
      httpUserLbl:'Login user', httpPassLbl:'Login password', repeatLbl:'Default repeats (1-10)',
      localeLbl:'Language', saveSettings:'Save settings',
      applyHint:'WiFi / AP / login changes apply after restart.',
      dataTitle:'Backup & restore', exportJson:'Export JSON', restoreJson:'Restore JSON',
      restart:'Restart', factoryReset:'Factory reset', diagTitle:'Diagnostics',
      loading:'Loading…', refresh:'Refresh',
      saved:'Saved', sent:'Sent', failed:'Request failed',
      confirmDeleteProfile:'Delete this profile?', confirmDeleteSignal:'Delete this button?',
      confirmFactory:'Factory reset? This erases all profiles and settings.',
      restartNotice:'Settings saved. Restart to apply Wi-Fi / login changes.',
      factoryHint:'Factory reset started. Reconnect to IHR-IR-Setup after reboot.',
      setupTitle:'First-time setup', setupStep1:'Connect your phone to the IHR-IR-Setup Wi-Fi network.',
      setupStep2:'Open Settings and enter your Wi-Fi name and password.',
      setupStep3:'Save, restart, then reconnect your phone to your own Wi-Fi.',
      setupGoSettings:'Open Settings',
      confirmRestart:'Restart the device now?', restored:'Backup restored',
      exported:'Backup downloaded', noProfiles:'Create a profile first',
      protocol:'Protocol', hex:'HEX value', bits:'Bits', raw:'Raw timings (µs)',
      label:'Label', labelBn:'Label (Bangla)', icon:'Icon', color:'Color', kind:'Kind',
      value:'Value (hex)', khz:'Carrier (kHz)', timings:'Timings (comma separated)',
      signalProtocol:'Protocol code (HEX + bits)', signalRaw:'Raw timing capture',
      editSignal:'Edit button', addSignalTitle:'Add button', editProfile:'Edit profile',
      addProfileTitle:'New profile', name:'Name', nameBn:'Name (Bangla)',
      powerHint:'Example codes must be verified with your own remote using Learn.',
      diagFirmware:'Firmware', diagUptime:'Uptime (s)', diagHeap:'Free heap (B)',
      diagFs:'Filesystem used / total (B)', diagIp:'IP address', diagWifi:'Mode',
      diagRssi:'RSSI (dBm)', diagReset:'Last reset', diagLocale:'Locale'
    },
    bn: {
      appName:'আইএইচআর আইআর রিমোট', connecting:'সংযোগ হচ্ছে…', online:'অনলাইন', apMode:'এপি মোড',
      tabRemotes:'রিমোট', tabLearn:'শিখুন', tabSettings:'সেটিংস', tabDiag:'ডায়াগনস্টিকস',
      addProfile:'+ প্রোফাইল', edit:'সম্পাদনা', duplicate:'নকল', delete:'মুছুন',
      emptyHint:'এখনো বাটন নেই। যোগ করুন বা রিমোট থেকে শিখুন।', addSignal:'+ বাটন যোগ',
      learnTitle:'সংকেত শিখুন', save:'সংরক্ষণ', cancel:'বাতিল', startLearning:'শিখতে শুরু করুন',
      learning:'শিখছে… আপনার রিমোটের বাটন চাপুন', saveAsButton:'বাটন হিসেবে সংরক্ষণ',
      learnHelp:'রিমোটটি TSOP38238 রিসিভারের দিকে তাক করে বাটন চাপুন। প্রোটোকল, হেক্স মান, বিট সংখ্যা ও কাঁচা টাইমিং নিচে দেখা যাবে।',
      learnIdle:'এখনো কিছু ধরা পড়েনি। শিখতে শুরু করে রিমোট বাটন চাপুন।',
      settingsTitle:'সেটিংস', deviceNameLbl:'ডিভাইসের নাম', apSsidLbl:'কনফিগ এপি এসএসআইডি',
      apPassLbl:'কনফিগ এপি পাসওয়ার্ড (খালি = ওপেন এপি)', wifiSsidLbl:'ওয়াইফাই নেটওয়ার্ক',
      wifiPassLbl:'ওয়াইফাই পাসওয়ার্ড', captiveLbl:'ক্যাপ্টিভ পোর্টাল', httpAuthLbl:'লগইন প্রয়োজন',
      httpUserLbl:'লগইন ব্যবহারকারী', httpPassLbl:'লগইন পাসওয়ার্ড', repeatLbl:'ডিফল্ট পুনরাবৃত্তি (১-১০)',
      localeLbl:'ভাষা', saveSettings:'সেটিংস সংরক্ষণ',
      applyHint:'ওয়াইফাই / এপি / লগইন পরিবর্তন রিস্টার্টের পর প্রয়োগ হবে।',
      dataTitle:'ব্যাকআপ ও পুনরুদ্ধার', exportJson:'JSON রপ্তানি', restoreJson:'JSON পুনরুদ্ধার',
      restart:'রিস্টার্ট', factoryReset:'ফ্যাক্টরি রিসেট', diagTitle:'ডায়াগনস্টিকস',
      loading:'লোড হচ্ছে…', refresh:'রিফ্রেশ',
      saved:'সংরক্ষিত', sent:'পাঠানো হয়েছে', failed:'অনুরোধ ব্যর্থ',
      confirmDeleteProfile:'এই প্রোফাইল মুছে ফেলবেন?', confirmDeleteSignal:'এই বাটন মুছে ফেলবেন?',
      confirmFactory:'ফ্যাক্টরি রিসেট? সব প্রোফাইল ও সেটিংস মুছে যাবে।',
      restartNotice:'সেটিংস সংরক্ষিত। ওয়াইফাই / লগইন পরিবর্তন প্রয়োগ করতে রিস্টার্ট করুন।',
      factoryHint:'ফ্যাক্টরি রিসেট শুরু হয়েছে। রিস্টার্টের পর IHR-IR-Setup-এ আবার কানেক্ট করুন।',
      setupTitle:'প্রথমবার সেটআপ', setupStep1:'আপনার ফোনকে IHR-IR-Setup ওয়াইফাইতে কানেক্ট করুন।',
      setupStep2:'সেটিংস খুলে আপনার ওয়াইফাইয়ের নাম ও পাসওয়ার্ড দিন।',
      setupStep3:'সংরক্ষণ করুন, রিস্টার্ট দিন, তারপর ফোনকে নিজের ওয়াইফাইতে কানেক্ট করুন।',
      setupGoSettings:'সেটিংস খুলুন',
      confirmRestart:'এখনই ডিভাইস রিস্টার্ট করবেন?', restored:'ব্যাকআপ পুনরুদ্ধার হয়েছে',
      exported:'ব্যাকআপ ডাউনলোড হয়েছে', noProfiles:'প্রথমে একটি প্রোফাইল তৈরি করুন',
      protocol:'প্রোটোকল', hex:'হেক্স মান', bits:'বিট', raw:'কাঁচা টাইমিং (µs)',
      label:'লেবেল', labelBn:'লেবেল (বাংলা)', icon:'আইকন', color:'রঙ', kind:'ধরন',
      value:'মান (হেক্স)', khz:'ক্যারিয়ার (kHz)', timings:'টাইমিং (কামা দিয়ে)',
      signalProtocol:'প্রোটোকল কোড (হেক্স + বিট)', signalRaw:'কাঁচা টাইমিং ক্যাপচার',
      editSignal:'বাটন সম্পাদনা', addSignalTitle:'বাটন যোগ', editProfile:'প্রোফাইল সম্পাদনা',
      addProfileTitle:'নতুন প্রোফাইল', name:'নাম', nameBn:'নাম (বাংলা)',
      powerHint:'উদাহরণ কোডগুলি Learn দিয়ে নিজের রিমোটে যাচাই করুন।',
      diagFirmware:'ফার্মওয়্যার', diagUptime:'আপটাইম (সে)', diagHeap:'মুক্ত হিপ (B)',
      diagFs:'ফাইলসিস্টেম ব্যবহৃত / মোট (B)', diagIp:'আইপি ঠিকানা', diagWifi:'মোড',
      diagRssi:'RSSI (dBm)', diagReset:'শেষ রিসেট', diagLocale:'ভাষা'
    }
  };

  var state = { cfg:null, db:null, status:null, learned:null, lang:'en',
                poll:null, modalMode:null, modalProfile:null, modalSignal:null };
  var $ = function (id) { return document.getElementById(id); };
  function t(k) { return (STR[state.lang] && STR[state.lang][k]) || (STR.en[k]) || k; }
  function esc(s) { return String(s == null ? '' : s).replace(/[&<>"']/g, function (c) {
    return {'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c];
  }); }

  function setText(el, key) {
    var v = t(key);
    if (el.childElementCount === 0) el.textContent = v;
    else if (el.firstChild && el.firstChild.nodeType === 3) el.firstChild.nodeValue = v;
  }
  function applyI18n() {
    document.documentElement.lang = state.lang;
    var nodes = document.querySelectorAll('[data-i18n]');
    for (var i = 0; i < nodes.length; i++) setText(nodes[i], nodes[i].getAttribute('data-i18n'));
  }

  var toastTimer = null;
  function toast(msg, isErr) {
    var el = $('toast');
    el.textContent = msg;
    el.className = 'toast' + (isErr ? ' err' : '');
    if (toastTimer) clearTimeout(toastTimer);
    toastTimer = setTimeout(function () { el.className = 'toast hidden'; }, 2800);
  }

  function showRestartNotice(show) {
    $('restartNotice').classList.toggle('hidden', !show);
    $('noticeRestartBtn').classList.toggle('hidden', !show);
  }

  function api(path, opts) {
    return fetch(path, opts).then(function (r) {
      if (!r.ok) throw new Error('HTTP ' + r.status);
      var ct = r.headers.get('content-type') || '';
      return ct.indexOf('json') >= 0 ? r.json() : r.text();
    });
  }
  function post(path, obj) {
    return api(path, { method:'POST', headers:{'Content-Type':'application/json'},
                       body: JSON.stringify(obj || {}) });
  }

  function loadAll() {
    return Promise.all([api('/api/config'), api('/api/signals'), api('/api/status')])
      .then(function (r) {
        state.cfg = r[0]; state.db = r[1]; state.status = r[2];
        if (!state.db || !Array.isArray(state.db.profiles)) state.db = { version:1, profiles:[] };
        state.lang = state.cfg.locale === 'bn' ? 'bn' : 'en';
        applyI18n(); fillConfig(); renderAll();
      })
      .catch(function (e) { toast(t('failed') + ': ' + e.message, true); });
  }

  function currentProfile() {
    if (!state.db || !state.db.profiles.length) return null;
    var id = $('profileSelect').value;
    for (var i = 0; i < state.db.profiles.length; i++)
      if (state.db.profiles[i].id === id) return state.db.profiles[i];
    return state.db.profiles[0];
  }
  function currentLangField(obj, base, lang) {
    return (obj[base + (lang === 'bn' ? 'Bn' : '')] || obj[base] || '');
  }

  function renderProfiles() {
    var sel = $('profileSelect');
    sel.innerHTML = '';
    var profiles = state.db.profiles;
    for (var i = 0; i < profiles.length; i++) {
      var o = document.createElement('option');
      o.value = profiles[i].id;
      o.textContent = currentLangField(profiles[i], 'name', state.lang) || profiles[i].id;
      sel.appendChild(o);
    }
    $('emptyHint').classList.toggle('hidden', profiles.length > 0 || !currentProfile());
  }

  function renderSignals() {
    var grid = $('signalGrid');
    grid.innerHTML = '';
    var p = currentProfile();
    if (!p) { $('emptyHint').classList.remove('hidden'); return; }
    var signals = p.signals || [];
    $('emptyHint').classList.toggle('hidden', signals.length > 0);
    for (var i = 0; i < signals.length; i++) {
      (function (s) {
        var btn = document.createElement('button');
        btn.type = 'button';
        btn.className = 'sig';
        btn.style.background = s.color || '#1b2742';
        btn.setAttribute('aria-label', currentLangField(s, 'label', state.lang));
        btn.innerHTML = '<span class="ic">' + esc(s.icon || '⭐') + '</span>' +
                        '<span class="lb">' + esc(currentLangField(s, 'label', state.lang)) + '</span>' +
                        '<span class="edit" role="button" aria-label="' + t('edit') + '">✎</span>';
        btn.addEventListener('click', function (ev) {
          if (ev.target && ev.target.classList.contains('edit')) { openSignalEditor(s); return; }
          sendSignal(s, ev);
        });
        grid.appendChild(btn);
      })(signals[i]);
    }
  }

  function renderStatus() {
    var s = state.status || {};
    var pill = $('connPill');
    var connectedSsid = (state.cfg && state.cfg.wifiSsid) || s.wifiSsid || '';
    pill.textContent = s.apMode ? t('apMode')
      : (s.wifiConnected ? (connectedSsid || t('online')) : t('connecting'));
    pill.title = s.apMode ? (s.apSsid || t('apMode'))
      : (s.wifiConnected ? (s.ip || connectedSsid) : t('connecting'));
    pill.className = 'pill ' + (s.apMode ? 'ap' : (s.wifiConnected ? 'on' : ''));
    var needSetup = !!s.apMode || !(state.cfg && state.cfg.wifiSsid);
    $('setupCard').classList.toggle('hidden', !needSetup);
    if (state.cfg && $('deviceName').textContent !== state.cfg.deviceName)
      $('deviceName').textContent = state.cfg.deviceName;

    if (!$('diagOut').classList.contains('hidden')) {
      var rows = [
        [t('diagFirmware'), s.version], [t('diagUptime'), s.uptimeSec],
        [t('diagHeap'), s.freeHeap], [t('diagFs'), s.fsUsed + ' / ' + s.fsTotal],
        [t('diagIp'), s.ip], [t('diagWifi'), s.apMode ? t('apMode') : (connectedSsid || 'STA')],
        [t('diagRssi'), s.rssi], [t('diagReset'), s.resetReason], [t('diagLocale'), s.locale]
      ];
      $('diagOut').textContent = rows.map(function (r) { return r[0] + ': ' + r[1]; }).join('\n');
    }
  }
  function renderAll() { renderProfiles(); renderSignals(); renderStatus(); }

  function saveDb() {
    return api('/api/signals', { method:'PUT', headers:{'Content-Type':'application/json'},
                                 body: JSON.stringify(state.db) }).then(function () {
      toast(t('saved'));
    }).catch(function (e) { toast(t('failed') + ': ' + e.message, true); });
  }

  function sendSignal(s, ev) {
    var p = currentProfile();
    var btn = ev && ev.target ? ev.target : null;
    if (btn) btn.classList.add('busy');
    post('/api/send', { profileId:p.id, signalId:s.id })
      .then(function () { toast(t('sent')); })
      .catch(function (e) { toast(t('failed') + ': ' + e.message, true); })
      .then(function () { if (btn) btn.classList.remove('busy'); });
  }

  function uid(prefix) {
    return prefix + '_' + Date.now().toString(36) + Math.floor(Math.random() * 1296).toString(36);
  }

  function closeModal() { $('modal').classList.add('hidden'); state.modalMode = null; }
  function modalHtml(title, fields) {
    return '<h2>' + esc(title) + '</h2>' + fields;
  }
  function fieldInput(id, label, value, type, attrs) {
    return '<label>' + esc(label) + '<input id="' + id + '" type="' + (type||'text') +
           '" value="' + esc(value || '') + '" ' + (attrs||'') + '></label>';
  }
  function fieldSelect(id, label, options, selected) {
    var s = '<label>' + esc(label) + '<select id="' + id + '">';
    for (var i = 0; i < options.length; i++)
      s += '<option value="' + esc(options[i]) + '"' +
           (options[i] === selected ? ' selected' : '') + '>' + esc(options[i]) + '</option>';
    return s + '</select></label>';
  }
  function protocolSelect(value) {
    var current = String(value || 'NEC').toUpperCase();
    var options = PROTOCOLS.slice();
    if (options.indexOf(current) < 0) options.push(current);
    return fieldSelect('mProtocol', t('protocol'), options, current);
  }

  function openProfileEditor(profile) {
    state.modalMode = 'profile';
    state.modalProfile = profile || null;
    var p = profile || { name:'', nameBn:'', icon:'📺', color:'#4f8cff' };
    $('modalBody').innerHTML = modalHtml(profile ? t('editProfile') : t('addProfileTitle'),
      fieldInput('mName', t('name'), p.name, 'text', 'maxlength="39"') +
      fieldInput('mNameBn', t('nameBn'), p.nameBn, 'text', 'maxlength="39"') +
      fieldSelect('mIcon', t('icon'), ICONS, p.icon) +
      fieldInput('mColor', t('color'), p.color, 'color'));
    $('modalDelete').classList.add('hidden');
    $('modal').classList.remove('hidden');
  }

  function openSignalEditor(signal) {
    state.modalMode = 'signal';
    state.modalSignal = signal || null;
    var s = signal || { label:'', labelBn:'', icon:'⏻', color:'#4f8cff', kind:'protocol',
                        protocol:'NEC', value:'0x00FF00FF', bits:32, repeat:1,
                        khz:38, timings:[] };
    var kindOpts = [t('signalProtocol'), t('signalRaw')];
    var sel = '<label>' + t('kind') + '<select id="mKind">';
    sel += '<option value="protocol"' + (s.kind === 'protocol' ? ' selected' : '') + '>' + kindOpts[0] + '</option>';
    sel += '<option value="raw"' + (s.kind === 'raw' ? ' selected' : '') + '>' + kindOpts[1] + '</option></select></label>';
    $('modalBody').innerHTML = modalHtml(signal ? t('editSignal') : t('addSignalTitle'),
      fieldInput('mLabel', t('label'), s.label, 'text', 'maxlength="39"') +
      fieldInput('mLabelBn', t('labelBn'), s.labelBn, 'text', 'maxlength="39"') +
      fieldSelect('mIcon', t('icon'), ICONS, s.icon) +
      fieldInput('mColor', t('color'), s.color, 'color') +
      sel +
      '<div id="mProtoFields">' +
        protocolSelect(s.protocol) +
        fieldInput('mValue', t('value'), typeof s.value === 'number' ? '0x' + s.value.toString(16) : s.value) +
        fieldInput('mBits', t('bits'), s.bits, 'number', 'min="1" max="64"') +
      '</div>' +
      '<div id="mRawFields">' +
        fieldInput('mKhz', t('khz'), s.khz || 38, 'number', 'min="30" max="60"') +
        '<label>' + t('timings') + '<textarea id="mTimings">' +
          esc((s.timings || []).join(', ')) + '</textarea></label>' +
      '</div>' +
      fieldInput('mRepeat', t('repeatLbl'), s.repeat || 1, 'number', 'min="1" max="10"') +
      '<p class="muted small">' + t('powerHint') + '</p>');
    function toggleKind() {
      var raw = $('mKind').value === 'raw';
      $('mProtoFields').classList.toggle('hidden', raw);
      $('mRawFields').classList.toggle('hidden', !raw);
    }
    $('mKind').addEventListener('change', toggleKind);
    toggleKind();
    $('modalDelete').classList.toggle('hidden', !signal);
    $('modal').classList.remove('hidden');
  }

  function modalSave() {
    if (state.modalMode === 'profile') {
      var name = $('mName').value.trim();
      if (!name) { toast(t('name') + '?', true); return; }
      var p = state.modalProfile || { id:uid('p'), signals:[] };
      p.name = name; p.nameBn = $('mNameBn').value.trim();
      p.icon = $('mIcon').value; p.color = $('mColor').value;
      if (!state.modalProfile) state.db.profiles.push(p);
      saveDb().then(renderAll); closeModal(); return;
    }
    if (state.modalMode === 'signal') {
      var prof = currentProfile();
      if (!prof) { toast(t('noProfiles'), true); return; }
      var s = state.modalSignal || { id:uid('s') };
      var kind = $('mKind').value;
      s.label = $('mLabel').value.trim() || 'Button';
      s.labelBn = $('mLabelBn').value.trim();
      s.icon = $('mIcon').value; s.color = $('mColor').value;
      s.kind = kind; s.repeat = Math.max(1, Math.min(10, parseInt($('mRepeat').value, 10) || 1));
      if (kind === 'protocol') {
        s.protocol = $('mProtocol').value.trim().toUpperCase() || 'NEC';
        var rawVal = $('mValue').value.trim();
        var num = (/^0x/i.test(rawVal) ? parseInt(rawVal, 16) : parseInt(rawVal, 10));
        if (isNaN(num) || num < 0) { toast(t('value') + '?', true); return; }
        s.value = num; s.bits = Math.max(1, Math.min(64, parseInt($('mBits').value, 10) || 32));
        delete s.khz; delete s.timings;
      } else {
        s.khz = Math.max(30, Math.min(60, parseInt($('mKhz').value, 10) || 38));
        var arr = $('mTimings').value.split(/[\s,;]+/).filter(function (x) { return x !== ''; })
                    .map(function (x) { return parseInt(x, 10); })
                    .filter(function (x) { return !isNaN(x) && x >= 0 && x <= 65535; })
                    .slice(0, 128);
        if (arr.length < 4) { toast(t('timings') + '?', true); return; }
        s.timings = arr; delete s.protocol; delete s.value; delete s.bits;
      }
      if (!state.modalSignal) { prof.signals = prof.signals || []; prof.signals.push(s); }
      saveDb().then(renderAll); closeModal();
    }
  }

  function fillConfig() {
    var c = state.cfg; if (!c) return;
    $('cfgDeviceName').value = c.deviceName || '';
    $('cfgApSsid').value = c.apSsid || '';
    $('cfgApPass').value = c.apPassword || '';
    $('cfgWifiSsid').value = c.wifiSsid || '';
    $('cfgWifiPass').value = c.wifiPassword || '';
    $('cfgCaptive').checked = !!c.captivePortal;
    $('cfgAuth').checked = !!c.httpAuth;
    $('cfgUser').value = c.httpUser || '';
    $('cfgHttpPass').value = c.httpPassword || '';
    $('cfgRepeat').value = c.irRepeat || 1;
    $('cfgLocale').value = (c.locale === 'bn') ? 'bn' : 'en';
  }

  function learnResultHtml(l) {
    var lines = [t('protocol') + ': ' + (l.protocol || 'UNKNOWN'),
                 t('hex') + ': ' + (l.hex || '—'),
                 t('bits') + ': ' + (l.bits || 0),
                 t('khz') + ': ' + (l.khz || 38)];
    if (l.timings && l.timings.length)
      lines.push(t('raw') + ':\n' + l.timings.join(', '));
    return esc(lines.join('\n'));
  }

  function pollLearn() {
    if (!state.poll) return;
    api('/api/learn/status').then(function (r) {
      if (!r || !r.active) {
        clearInterval(state.poll); state.poll = null;
        $('learnStart').classList.remove('hidden');
        $('learnCancel').classList.add('hidden');
        if (r && r.capture && r.capture.protocol) {
          state.learned = r.capture;
          $('learnResult').textContent = learnResultHtml(r.capture);
          $('learnSave').classList.remove('hidden');
        }
      }
    }).catch(function () { clearInterval(state.poll); state.poll = null; });
  }

  function bind() {
    var tabs = document.querySelectorAll('.tab');
    for (var i = 0; i < tabs.length; i++) tabs[i].addEventListener('click', function () {
      var name = this.getAttribute('data-tab');
      for (var j = 0; j < tabs.length; j++) tabs[j].classList.remove('active');
      this.classList.add('active');
      var panels = document.querySelectorAll('.panel');
      for (var k = 0; k < panels.length; k++) panels[k].classList.remove('active');
      $('tab-' + name).classList.add('active');
      if (name === 'diag') { $('diagOut').classList.remove('hidden'); api('/api/status').then(function (s) {
        state.status = s; renderStatus();
      }); }
    });

    $('langBtn').addEventListener('click', function () {
      state.lang = state.lang === 'en' ? 'bn' : 'en';
      state.cfg.locale = state.lang; applyI18n(); renderAll();
      post('/api/config', { locale:state.lang }).catch(function () {});
    });
    $('profileSelect').addEventListener('change', renderSignals);
    $('addProfileBtn').addEventListener('click', function () { openProfileEditor(null); });
    $('editProfileBtn').addEventListener('click', function () {
      var p = currentProfile(); if (p) openProfileEditor(p); else toast(t('noProfiles'), true);
    });
    $('dupProfileBtn').addEventListener('click', function () {
      var p = currentProfile(); if (!p) return;
      var c = JSON.parse(JSON.stringify(p));
      c.id = uid('p'); c.name = (c.name || '') + ' copy'; c.nameBn = (c.nameBn || '') + ' (কপি)';
      for (var i = 0; i < (c.signals || []).length; i++) c.signals[i].id = uid('s');
      state.db.profiles.push(c); saveDb().then(function () {
        $('profileSelect').value = c.id; renderAll();
      });
    });
    $('delProfileBtn').addEventListener('click', function () {
      var p = currentProfile(); if (!p || !confirm(t('confirmDeleteProfile'))) return;
      state.db.profiles = state.db.profiles.filter(function (x) { return x.id !== p.id; });
      saveDb().then(renderAll);
    });
    $('addSignalBtn').addEventListener('click', function () {
      if (!currentProfile()) { toast(t('noProfiles'), true); return; }
      openSignalEditor(null);
    });

    $('modalSave').addEventListener('click', modalSave);
    $('modalCancel').addEventListener('click', closeModal);
    $('modalDelete').addEventListener('click', function () {
      if (state.modalMode !== 'signal' || !state.modalSignal) return;
      if (!confirm(t('confirmDeleteSignal'))) return;
      var p = currentProfile(); if (!p) return;
      p.signals = (p.signals || []).filter(function (x) { return x.id !== state.modalSignal.id; });
      saveDb().then(renderAll); closeModal();
    });
    $('setupGoSettings').addEventListener('click', function () {
      var tab = document.querySelector('[data-tab="settings"]');
      if (tab) tab.click();
    });
    $('noticeRestartBtn').addEventListener('click', function () {
      if (!confirm(t('confirmRestart'))) return;
      post('/api/restart', {}).catch(function () {}).then(function () { location.reload(); });
    });

    $('learnStart').addEventListener('click', function () {
      post('/api/learn/start', {}).then(function () {
        $('learnResult').textContent = t('learning');
        $('learnResult').classList.remove('hidden');
        $('learnSave').classList.add('hidden');
        $('learnStart').classList.add('hidden');
        $('learnCancel').classList.remove('hidden');
        if (state.poll) clearInterval(state.poll);
        state.poll = setInterval(pollLearn, 500);
      }).catch(function (e) { toast(t('failed') + ': ' + e.message, true); });
    });
    $('learnCancel').addEventListener('click', function () {
      if (state.poll) { clearInterval(state.poll); state.poll = null; }
      post('/api/learn/cancel', {}).then(function () {
        $('learnStart').classList.remove('hidden');
        $('learnCancel').classList.add('hidden');
        $('learnResult').textContent = t('learnIdle');
      });
    });
    $('learnSave').addEventListener('click', function () {
      if (!currentProfile()) { toast(t('noProfiles'), true); return; }
      var l = state.learned; if (!l) return;
      var s = l.kind === 'raw'
        ? { kind:'raw', protocol:'RAW', khz:l.khz, timings:l.timings, value:l.hex }
        : { kind:'protocol', protocol:l.protocol, value:l.hex, bits:l.bits };
      s.label = l.protocol || 'Learned'; s.icon = '⭐'; s.color = '#4f8cff'; s.repeat = 1;
      openSignalEditor(s);
      state.learned = null;
    });

    $('configForm').addEventListener('submit', function (ev) {
      ev.preventDefault();
      var c = {
        deviceName:$('cfgDeviceName').value.trim(),
        apSsid:$('cfgApSsid').value.trim(), apPassword:$('cfgApPass').value,
        wifiSsid:$('cfgWifiSsid').value.trim(), wifiPassword:$('cfgWifiPass').value,
        captivePortal:$('cfgCaptive').checked, httpAuth:$('cfgAuth').checked,
        httpUser:$('cfgUser').value.trim(), httpPassword:$('cfgHttpPass').value,
        irRepeat:Math.max(1, Math.min(10, parseInt($('cfgRepeat').value, 10) || 1)),
        locale:$('cfgLocale').value
      };
      post('/api/config', c).then(function (r) {
        state.cfg = c; state.lang = c.locale === 'bn' ? 'bn' : 'en';
        applyI18n(); toast(t('saved')); showRestartNotice(!!(r && r.restartRequired)); loadAll();
      }).catch(function (e) { toast(t('failed') + ': ' + e.message, true); });
    });

    $('exportBtn').addEventListener('click', function () {
      var payload = { format:'ihr-backup', version:1, config:state.cfg, signals:state.db };
      var blob = new Blob([JSON.stringify(payload, null, 2)], {type:'application/json'});
      var a = document.createElement('a');
      a.href = URL.createObjectURL(blob);
      a.download = 'ihr-backup-' + new Date().toISOString().slice(0,10) + '.json';
      a.click(); setTimeout(function () { URL.revokeObjectURL(a.href); }, 1000);
      toast(t('exported'));
    });
    $('restoreBtn').addEventListener('click', function () { $('restoreFile').click(); });
    $('restoreFile').addEventListener('change', function () {
      var f = this.files && this.files[0]; if (!f) return;
      var reader = new FileReader();
      reader.onload = function () {
        try {
          var d = JSON.parse(reader.result);
          if (d.format !== 'ihr-backup' || !d.config || !d.signals) throw new Error('bad file');
          var needsRestart = false;
          post('/api/config', d.config)
            .then(function (r) {
              needsRestart = !!(r && r.restartRequired);
              return api('/api/signals', { method:'PUT',
                headers:{'Content-Type':'application/json'}, body:JSON.stringify(d.signals) });
            })
            .then(function () { toast(t('restored')); showRestartNotice(needsRestart); loadAll(); })
            .catch(function (e) { toast(t('failed') + ': ' + e.message, true); });
        } catch (e) { toast(t('failed') + ': ' + e.message, true); }
      };
      reader.readAsText(f);
      this.value = '';
    });

    $('restartBtn').addEventListener('click', function () {
      if (!confirm(t('confirmRestart'))) return;
      post('/api/restart', {}).then(function () { location.reload(); }).catch(function () { location.reload(); });
    });
    $('factoryBtn').addEventListener('click', function () {
      if (!confirm(t('confirmFactory'))) return;
      post('/api/factory-reset', {})
        .then(function () { alert(t('factoryHint')); })
        .catch(function () { alert(t('factoryHint')); })
        .then(function () {
          if (state.status && state.status.apMode) {
            setTimeout(function () { location.assign('http://192.168.4.1/'); }, 2500);
          }
        });
    });
    $('diagRefresh').addEventListener('click', function () {
      api('/api/status').then(function (s) { state.status = s; renderStatus(); });
    });
  }

  showRestartNotice(false);
  bind();
  loadAll();
  setInterval(function () { api('/api/status').then(function (s) {
    state.status = s; renderStatus();
  }).catch(function () {}); }, 5000);
})();
