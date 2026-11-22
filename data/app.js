/* ROVIXANE IR — Universal Infrared Remote - offline dashboard. No external dependencies. */
(function () {
  'use strict';

  var ICONS = ['⏻','📺','🔊','🔈','🔇','➕','➖','⬆','⬇','⬅','➡','✅',
               '↩','🏠','☰','▶','⏸','⏹','⏮','⏭','❄','💡','🌀','⭐'];
  var COLORS = ['#4f8cff','#ff6b6b','#37d67a','#ffc857','#b06dff','#00c2c7','#e8eefc'];
  var PROTOCOLS = ['NEC','LG','LG2','SONY','SAMSUNG','JVC','PANASONIC',
                   'RC5','RC6','DENON','COOLIX','MIDEA24'];

  var STR = {
    en: {
      appName:'ROVIXANE IR',
      appTitle:'ROVIXANE IR — Universal Infrared Remote',
      metaDescription:'Offline universal infrared remote dashboard for NodeMCU ESP8266 ESP-12E.',
      connecting:'Connecting to Wi-Fi…', online:'Home Wi-Fi', apMode:'Setup AP',
      tabRemotes:'Remotes', tabLearn:'Learn signal', tabSettings:'Settings', tabDiag:'Diagnostics',
      addProfile:'+ Profile', edit:'Edit', duplicate:'Duplicate', delete:'Delete',
      emptyHint:'No buttons yet. Add a button or learn one from your remote.',
      addSignal:'+ Add button',
      learnTitle:'Learn remote signal', save:'Save', cancel:'Cancel',
      startLearning:'Start learning',
      learning:'Receiving signal… press a button on your remote',
      saveAsButton:'Save as button',
      learnHelp:'Point your remote at the TSOP38238 receiver and press a button. The dashboard shows the decoded protocol, HEX value, bit length, carrier frequency and Raw timing capture.',
      learnIdle:'No signal captured yet. Start learning, then press a remote button.',
      learnNoCapture:'No usable signal was captured. Try again closer to the receiver.',
      unknownProtocol:'UNKNOWN', learnedButtonLabel:'Learned signal',
      defaultButtonLabel:'New button',
      settingsTitle:'Settings', deviceNameLbl:'Device name',
      apSsidLbl:'Setup AP name (SSID)',
      apPassLbl:'Setup AP password (empty = open AP)',
      wifiSsidLbl:'Wi-Fi network (SSID)', wifiPassLbl:'Wi-Fi password',
      captiveLbl:'Captive portal', httpAuthLbl:'Require dashboard login',
      httpUserLbl:'Login user', httpPassLbl:'Login password',
      repeatLbl:'Default repeats (1–10)', localeLbl:'Interface language',
      langEnglish:'English', langBangla:'Bangla', saveSettings:'Save settings',
      applyHint:'Wi-Fi, AP and login changes apply after a restart.',
      dataTitle:'Backup & restore',
      backupHint:'Export a JSON backup before changing settings or updating firmware.',
      exportJson:'Download backup', restoreJson:'Restore backup',
      restart:'Restart', factoryReset:'Factory reset', diagTitle:'Diagnostics',
      diagHint:'Live status reported by the device.',
      loading:'Loading…', refresh:'Refresh',
      saved:'Saved', sent:'Signal sent', requestFailed:'Request failed',
      saveFailed:'Could not save. Check the connection and try again.',
      loadFailed:'Could not load dashboard data. Check the connection and refresh.',
      sendFailed:'Could not send the signal. Check the connection and wiring.',
      learnStartFailed:'Could not start learning. Please try again.',
      learnCancelFailed:'Could not stop learning. Please try again.',
      configSaveFailed:'Settings were not saved. Check the values and try again.',
      localeSaveFailed:'Language preference could not be saved on the device.',
      statusLoadFailed:'Could not refresh diagnostics.',
      backupExportFailed:'Backup could not be created on this device.',
      backupInvalid:'This file is not a valid ROVIXANE IR backup.',
      errBadRequest:'The entered values were rejected. Check the form.',
      errUnauthorized:'Login required. Check your username and password.',
      errForbidden:'Access to this device is not allowed.',
      errNotFound:'The requested page or API was not found.',
      errServer:'The device returned a server error. Try again shortly.',
      errNetwork:'The device did not respond. Check the connection.',
      statusCode:'Status code',
      confirmDeleteProfile:'Delete this profile? All of its buttons will be deleted too.',
      confirmDeleteSignal:'Delete this button?',
      confirmFactory:'Confirm factory reset? All profiles, buttons and settings will be erased.',
      restartNotice:'Settings saved. Restart to apply Wi-Fi or login changes.',
      factoryHint:'Factory reset started. After reboot, connect to the ROVIXANE-IR-Setup Wi-Fi network again.',
      setupTitle:'First-time setup',
      setupStep1:'Connect your phone to the ROVIXANE-IR-Setup Wi-Fi network.',
      setupStep2:'Open http://192.168.4.1 in your browser.',
      setupStep3:'Open Settings and enter your Wi-Fi network name and password.',
      setupStep4:'Save, restart, then reconnect your phone to your own Wi-Fi network.',
      setupNote:'If your phone says there is no internet, keep using this network so setup can continue.',
      setupGoSettings:'Open Settings', confirmRestart:'Restart the device now?',
      restarting:'Restarting…', exported:'Backup downloaded', restored:'Backup restored',
      noProfiles:'Create a profile first', copySuffix:' (copy)',
      protocol:'Protocol', hex:'HEX value', bits:'Bit length', raw:'Raw timing (µs)',
      label:'Button name', labelBn:'Button name (Bangla)', icon:'Icon', color:'Color',
      kind:'Signal type', value:'HEX value', khz:'Carrier frequency (kHz)',
      timings:'Raw timings (comma separated)',
      signalProtocol:'Protocol code (HEX + bits)', signalRaw:'Raw timing capture',
      editSignal:'Edit button', addSignalTitle:'New button', editProfile:'Edit profile',
      addProfileTitle:'New profile', name:'Name (English)', nameBn:'Name (Bangla)',
      powerHint:'Example codes must be verified with your own remote using Learn.',
      errNameRequired:'Enter a name before saving.',
      errHexValue:'Enter a valid HEX or decimal value.',
      errRawTimings:'Enter at least four Raw timing values.',
      errDeviceName:'Enter a device name.',
      errApSsid:'Enter a setup AP name.',
      errApPassword:'Setup AP password must be empty or 8–63 characters.',
      errWifiPassword:'Wi-Fi password must be 63 characters or fewer.',
      errHttpUser:'Enter a login user when login is enabled.',
      errHttpPassword:'Login password must be at least 4 characters.',
      diagFirmware:'Firmware', diagUptime:'Uptime (s)', diagHeap:'Free Heap memory (B)',
      diagFs:'File system used / total (B)', diagIp:'IP address', diagWifi:'Wi-Fi state',
      staMode:'Station mode', diagRssi:'RSSI (dBm)', diagReset:'Last reset reason',
      diagLocale:'Interface language',
      resetPower:'Power on', resetExternal:'External reset', resetSoftware:'Software restart',
      resetWatchdog:'Watchdog reset', resetDeepSleep:'Deep-sleep wake', resetUnknown:'Unknown',
      ariaLanguage:'Switch language', ariaMainNav:'Main navigation',
      ariaRemotesPanel:'Remote profiles panel', ariaLearnPanel:'Signal learning panel',
      ariaSettingsPanel:'Settings panel', ariaDiagnosticsPanel:'Diagnostics panel',
      ariaProfileSelect:'Select a remote profile', ariaEditButton:'Edit button',
      ariaModal:'Profile and button editor', ariaToast:'Notifications',
      footerText:'ROVIXANE IR — Universal Infrared Remote'
    },
    bn: {
      appName:'ROVIXANE IR',
      appTitle:'ROVIXANE IR — সার্বজনীন ইনফ্রারেড রিমোট',
      metaDescription:'NodeMCU ESP8266 ESP-12E-এর জন্য অফলাইন সার্বজনীন ইনফ্রারেড রিমোট ড্যাশবোর্ড।',
      connecting:'Wi-Fi নেটওয়ার্কের সাথে সংযোগ হচ্ছে…', online:'বাড়ির Wi-Fi', apMode:'সেটআপ এপি',
      tabRemotes:'রিমোট', tabLearn:'সংকেত শেখা', tabSettings:'সেটিংস', tabDiag:'ডায়াগনস্টিকস',
      addProfile:'+ প্রোফাইল', edit:'সম্পাদনা', duplicate:'অনুলিপি', delete:'মুছে ফেলুন',
      emptyHint:'এখনো কোনো বাটন নেই। বাটন যোগ করুন অথবা রিমোট থেকে সংকেত শিখুন।',
      addSignal:'+ বাটন যোগ',
      learnTitle:'রিমোটের সংকেত শেখা', save:'সংরক্ষণ করুন', cancel:'বাতিল করুন',
      startLearning:'শেখা শুরু করুন',
      learning:'সংকেত গ্রহণ চলছে… রিমোটের বাটন চাপুন',
      saveAsButton:'বাটন হিসেবে সংরক্ষণ',
      learnHelp:'রিমোটটি TSOP38238 রিসিভারের দিকে তাক করে বাটন চাপুন। ড্যাশবোর্ডে ডিকোড করা প্রোটোকল, HEX মান, বিট সংখ্যা, ক্যারিয়ার ফ্রিকোয়েন্সি ও Raw টাইমিং দেখা যাবে।',
      learnIdle:'এখনো কোনো সংকেত ধরা পড়েনি। শেখা শুরু করে রিমোটের বাটন চাপুন।',
      learnNoCapture:'ব্যবহারযোগ্য সংকেত ধরা পড়েনি। রিসিভারের কাছ থেকে আবার চেষ্টা করুন।',
      unknownProtocol:'অজানা', learnedButtonLabel:'শেখা সংকেত',
      defaultButtonLabel:'নতুন বাটন',
      settingsTitle:'সেটিংস', deviceNameLbl:'ডিভাইসের নাম',
      apSsidLbl:'সেটআপ এপির নাম (SSID)',
      apPassLbl:'সেটআপ এপির পাসওয়ার্ড (খালি = উন্মুক্ত এপি)',
      wifiSsidLbl:'Wi-Fi নেটওয়ার্ক (SSID)', wifiPassLbl:'Wi-Fi পাসওয়ার্ড',
      captiveLbl:'ক্যাপ্টিভ পোর্টাল', httpAuthLbl:'ড্যাশবোর্ডে লগইন প্রয়োজন',
      httpUserLbl:'লগইন ব্যবহারকারী', httpPassLbl:'লগইন পাসওয়ার্ড',
      repeatLbl:'ডিফল্ট পুনরাবৃত্তি (১–১০)', localeLbl:'ইন্টারফেসের ভাষা',
      langEnglish:'ইংরেজি', langBangla:'বাংলা', saveSettings:'সেটিংস সংরক্ষণ করুন',
      applyHint:'Wi-Fi, এপি বা লগইন পরিবর্তন প্রয়োগ করতে ডিভাইস রিস্টার্ট করুন।',
      dataTitle:'ব্যাকআপ ও পুনরুদ্ধার',
      backupHint:'সেটিংস বদলানোর আগে বা ফার্মওয়্যার আপডেটের আগে JSON ব্যাকআপ ডাউনলোড করুন।',
      exportJson:'ব্যাকআপ ডাউনলোড', restoreJson:'ব্যাকআপ পুনরুদ্ধার',
      restart:'রিস্টার্ট করুন', factoryReset:'ফ্যাক্টরি রিসেট', diagTitle:'ডায়াগনস্টিকস',
      diagHint:'ডিভাইসের বর্তমান অবস্থা এখানে দেখা যায়।',
      loading:'লোড হচ্ছে…', refresh:'রিফ্রেশ করুন',
      saved:'সংরক্ষণ হয়েছে', sent:'সংকেত পাঠানো হয়েছে', requestFailed:'অনুরোধ ব্যর্থ',
      saveFailed:'সংরক্ষণ করা যায়নি। সংযোগ যাচাই করে আবার চেষ্টা করুন।',
      loadFailed:'ড্যাশবোর্ডের তথ্য লোড করা যায়নি। সংযোগ যাচাই করে রিফ্রেশ করুন।',
      sendFailed:'সংকেত পাঠানো যায়নি। সংযোগ ও ওয়্যারিং যাচাই করুন।',
      learnStartFailed:'শেখা শুরু করা যায়নি। আবার চেষ্টা করুন।',
      learnCancelFailed:'শেখা বন্ধ করা যায়নি। আবার চেষ্টা করুন।',
      configSaveFailed:'সেটিংস সংরক্ষণ হয়নি। মানগুলো যাচাই করে আবার চেষ্টা করুন।',
      localeSaveFailed:'ভাষা পছন্দ ডিভাইসে সংরক্ষণ করা যায়নি।',
      statusLoadFailed:'ডায়াগনস্টিকস রিফ্রেশ করা যায়নি।',
      backupExportFailed:'এই ডিভাইসে ব্যাকআপ তৈরি করা যায়নি।',
      backupInvalid:'এই ফাইলটি বৈধ ROVIXANE IR ব্যাকআপ নয়।',
      errBadRequest:'দেওয়া মানগুলো গ্রহণ করা হয়নি। ফর্মটি যাচাই করুন।',
      errUnauthorized:'লগইন প্রয়োজন। ব্যবহারকারী ও পাসওয়ার্ড যাচাই করুন।',
      errForbidden:'এই ডিভাইসে প্রবেশের অনুমতি নেই।',
      errNotFound:'অনুরোধ করা পৃষ্ঠা বা API পাওয়া যায়নি।',
      errServer:'ডিভাইস সার্ভার ত্রুটি দিয়েছে। কিছুক্ষণ পর আবার চেষ্টা করুন।',
      errNetwork:'ডিভাইস থেকে কোনো সাড়া পাওয়া যায়নি। সংযোগ যাচাই করুন।',
      statusCode:'স্থিতি কোড',
      confirmDeleteProfile:'এই প্রোফাইলটি মুছে ফেলবেন? এর সব বাটনও মুছে যাবে।',
      confirmDeleteSignal:'এই বাটনটি মুছে ফেলবেন?',
      confirmFactory:'ফ্যাক্টরি রিসেট নিশ্চিত করুন? সব প্রোফাইল, বাটন ও সেটিংস মুছে যাবে।',
      restartNotice:'সেটিংস সংরক্ষণ হয়েছে। Wi-Fi বা লগইন পরিবর্তন প্রয়োগ করতে রিস্টার্ট করুন।',
      factoryHint:'ফ্যাক্টরি রিসেট শুরু হয়েছে। রিস্টার্টের পর ROVIXANE-IR-Setup Wi-Fi নেটওয়ার্কে আবার সংযোগ করুন।',
      setupTitle:'প্রথমবারের সেটআপ',
      setupStep1:'আপনার ফোনটিকে ROVIXANE-IR-Setup Wi-Fi নেটওয়ার্কের সাথে সংযোগ করুন।',
      setupStep2:'ব্রাউজারে http://192.168.4.1 খুলুন।',
      setupStep3:'সেটিংসে আপনার Wi-Fi নেটওয়ার্কের নাম ও পাসওয়ার্ড দিন।',
      setupStep4:'সংরক্ষণ করে রিস্টার্ট করুন; তারপর ফোনকে আপনার নিজের Wi-Fi নেটওয়ার্কে সংযোগ করুন।',
      setupNote:'ফোন যদি ইন্টারনেট নেই বলে বার্তা দেখায়, সেটআপ শেষ করতে এই নেটওয়ার্কেই থাকুন।',
      setupGoSettings:'সেটিংস খুলুন', confirmRestart:'এখন ডিভাইস রিস্টার্ট করবেন?',
      restarting:'রিস্টার্ট হচ্ছে…', exported:'ব্যাকআপ ডাউনলোড হয়েছে',
      restored:'ব্যাকআপ পুনরুদ্ধার হয়েছে', noProfiles:'প্রথমে একটি প্রোফাইল তৈরি করুন',
      copySuffix:' (কপি)',
      protocol:'প্রোটোকল', hex:'HEX মান', bits:'বিট সংখ্যা', raw:'Raw টাইমিং (µs)',
      label:'বাটনের নাম', labelBn:'বাটনের নাম (বাংলা)', icon:'আইকন', color:'রঙ',
      kind:'সংকেতের ধরন', value:'HEX মান', khz:'ক্যারিয়ার ফ্রিকোয়েন্সি (kHz)',
      timings:'Raw টাইমিং (কামা দিয়ে)',
      signalProtocol:'প্রোটোকল কোড (HEX + বিট)', signalRaw:'Raw টাইমিং ক্যাপচার',
      editSignal:'বাটন সম্পাদনা', addSignalTitle:'নতুন বাটন', editProfile:'প্রোফাইল সম্পাদনা',
      addProfileTitle:'নতুন প্রোফাইল', name:'নাম (ইংরেজি)', nameBn:'নাম (বাংলা)',
      powerHint:'উদাহরণ কোডগুলো সংকেত শেখা দিয়ে নিজের রিমোটে যাচাই করুন।',
      errNameRequired:'সংরক্ষণের আগে একটি নাম দিন।',
      errHexValue:'বৈধ HEX বা দশমিক মান দিন।',
      errRawTimings:'কমপক্ষে চারটি Raw টাইমিং মান দিন।',
      errDeviceName:'ডিভাইসের নাম দিন।',
      errApSsid:'সেটআপ এপির নাম দিন।',
      errApPassword:'সেটআপ এপির পাসওয়ার্ড খালি অথবা ৮–৬৩ অক্ষর হতে হবে।',
      errWifiPassword:'Wi-Fi পাসওয়ার্ড সর্বোচ্চ ৬৩ অক্ষর হতে পারে।',
      errHttpUser:'লগইন চালু থাকলে ব্যবহারকারীর নাম দিন।',
      errHttpPassword:'লগইন পাসওয়ার্ড কমপক্ষে ৪ অক্ষর হতে হবে।',
      diagFirmware:'ফার্মওয়্যার', diagUptime:'চলার সময় (সেকেন্ড)',
      diagHeap:'মুক্ত Heap মেমোরি (B)', diagFs:'ফাইল সিস্টেম — ব্যবহৃত / মোট (B)',
      diagIp:'IP ঠিকানা', diagWifi:'Wi-Fi অবস্থা', staMode:'স্টেশন মোড',
      diagRssi:'RSSI (dBm)', diagReset:'সর্বশেষ রিস্টার্টের কারণ',
      diagLocale:'ইন্টারফেসের ভাষা',
      resetPower:'পাওয়ার চালু', resetExternal:'বাহ্যিক রিসেট',
      resetSoftware:'সফটওয়্যার রিস্টার্ট', resetWatchdog:'ওয়াচডগ রিসেট',
      resetDeepSleep:'ডিপ-স্লিপ থেকে জাগরণ', resetUnknown:'অজানা',
      ariaLanguage:'ভাষা বদলান', ariaMainNav:'প্রধান নেভিগেশন',
      ariaRemotesPanel:'রিমোট প্রোফাইল প্যানেল', ariaLearnPanel:'সংকেত শেখার প্যানেল',
      ariaSettingsPanel:'সেটিংস প্যানেল', ariaDiagnosticsPanel:'ডায়াগনস্টিকস প্যানেল',
      ariaProfileSelect:'রিমোটের প্রোফাইল বেছে নিন', ariaEditButton:'বাটন সম্পাদনা',
      ariaModal:'প্রোফাইল ও বাটন সম্পাদনা', ariaToast:'বিজ্ঞপ্তি',
      footerText:'ROVIXANE IR — সার্বজনীন ইনফ্রারেড রিমোট'
    }
  };

  function storedLang() {
    try {
      var v = localStorage.getItem('rovixane-lang');
      return v === 'bn' || v === 'en' ? v : null;
    } catch (e) { return null; }
  }
  function rememberLang(lang) {
    try { localStorage.setItem('rovixane-lang', lang); } catch (e) { /* storage unavailable */ }
  }

  var state = { cfg:null, db:null, status:null, learned:null, lang:storedLang() || 'en',
                poll:null, modalMode:null, modalProfile:null, modalSignal:null,
                modalReturnFocus:null };
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
    var root = document.documentElement;
    root.lang = state.lang;
    root.setAttribute('data-lang', state.lang);
    document.title = t('appTitle');
    var meta = document.querySelector('meta[name="description"]');
    if (meta) meta.setAttribute('content', t('metaDescription'));

    var nodes = document.querySelectorAll('[data-i18n]');
    for (var i = 0; i < nodes.length; i++) setText(nodes[i], nodes[i].getAttribute('data-i18n'));
    var attrs = [['data-i18n-aria', 'aria-label'],
                 ['data-i18n-title', 'title'],
                 ['data-i18n-placeholder', 'placeholder']];
    for (var a = 0; a < attrs.length; a++) {
      var targets = document.querySelectorAll('[' + attrs[a][0] + ']');
      for (var j = 0; j < targets.length; j++)
        targets[j].setAttribute(attrs[a][1], t(targets[j].getAttribute(attrs[a][0])));
    }
  }

  var toastTimer = null;
  function toast(msg, isErr) {
    var el = $('toast');
    el.textContent = msg;
    el.className = 'toast' + (isErr ? ' err' : '') + ' show';
    el.setAttribute('aria-live', isErr ? 'assertive' : 'polite');
    if (toastTimer) clearTimeout(toastTimer);
    toastTimer = setTimeout(function () { el.className = 'toast hidden'; }, 3200);
  }

  function showRestartNotice(show) {
    $('restartNotice').classList.toggle('hidden', !show);
    $('noticeRestartBtn').classList.toggle('hidden', !show);
  }

  function statusError(status) {
    var suffix = ' (' + t('statusCode') + ': ' + status + ')';
    switch (status) {
      case 400: return t('errBadRequest') + suffix;
      case 401: return t('errUnauthorized') + suffix;
      case 403: return t('errForbidden') + suffix;
      case 404: return t('errNotFound') + suffix;
      default:
        if (status >= 500) return t('errServer') + suffix;
        return t('requestFailed') + suffix;
    }
  }
  function localizedError(message) {
    var e = new Error(message);
    e.localized = true;
    return e;
  }
  function api(path, opts) {
    return fetch(path, opts).then(function (r) {
      if (!r.ok) throw localizedError(statusError(r.status));
      var ct = r.headers.get('content-type') || '';
      return ct.indexOf('json') >= 0 ? r.json() : r.text();
    }).catch(function (e) {
      if (e && e.localized) throw e;
      throw localizedError(t('errNetwork'));
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
        rememberLang(state.lang);
        applyI18n(); fillConfig(); renderAll();
      })
      .catch(function () { toast(t('loadFailed'), true); });
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
        var label = currentLangField(s, 'label', state.lang) || t('defaultButtonLabel');
        var btn = document.createElement('button');
        btn.type = 'button';
        btn.className = 'sig';
        btn.style.background = s.color || '#1b2742';
        btn.title = label;
        btn.setAttribute('aria-label', label);
        btn.innerHTML = '<span class="ic">' + esc(s.icon || '⭐') + '</span>' +
                        '<span class="lb">' + esc(label) + '</span>' +
                        '<span class="edit" aria-hidden="true">✎</span>';
        btn.addEventListener('click', function (ev) {
          if (ev.target && ev.target.classList.contains('edit')) { openSignalEditor(s); return; }
          sendSignal(s, ev);
        });
        grid.appendChild(btn);
      })(signals[i]);
    }
  }

  function localeLabel(code) {
    if (code === 'bn') return t('langBangla');
    if (code === 'en') return t('langEnglish');
    return code || '—';
  }
  function resetLabel(value) {
    var v = String(value || '').toLowerCase();
    if (v.indexOf('power') >= 0) return t('resetPower');
    if (v.indexOf('external') >= 0 || v.indexOf('pin') >= 0) return t('resetExternal');
    if (v.indexOf('watchdog') >= 0) return t('resetWatchdog');
    if (v.indexOf('deep') >= 0 || v.indexOf('sleep') >= 0) return t('resetDeepSleep');
    if (v.indexOf('software') >= 0 || v.indexOf('system') >= 0) return t('resetSoftware');
    return t('resetUnknown');
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
      var wifiState = s.apMode ? t('apMode')
        : (s.wifiConnected ? (connectedSsid || t('staMode')) : t('connecting'));
      var rows = [
        [t('diagFirmware'), s.version], [t('diagUptime'), s.uptimeSec],
        [t('diagHeap'), s.freeHeap], [t('diagFs'), s.fsUsed + ' / ' + s.fsTotal],
        [t('diagIp'), s.ip], [t('diagWifi'), wifiState],
        [t('diagRssi'), s.rssi], [t('diagReset'), resetLabel(s.resetReason)],
        [t('diagLocale'), localeLabel(s.locale)]
      ];
      $('diagOut').textContent = rows.map(function (r) { return r[0] + ': ' + r[1]; }).join('\n');
    }
  }
  function renderAll() { renderProfiles(); renderSignals(); renderStatus(); }

  function saveDb() {
    return api('/api/signals', { method:'PUT', headers:{'Content-Type':'application/json'},
                                 body: JSON.stringify(state.db) }).then(function () {
      toast(t('saved'));
    }).catch(function () { toast(t('saveFailed'), true); });
  }

  function sendSignal(s, ev) {
    var p = currentProfile();
    var btn = ev && ev.target ? ev.target : null;
    if (btn) btn.classList.add('busy');
    post('/api/send', { profileId:p.id, signalId:s.id })
      .then(function () { toast(t('sent')); })
      .catch(function () { toast(t('sendFailed'), true); })
      .then(function () { if (btn) btn.classList.remove('busy'); });
  }

  function uid(prefix) {
    return prefix + '_' + Date.now().toString(36) + Math.floor(Math.random() * 1296).toString(36);
  }

  function openModal() {
    state.modalReturnFocus = document.activeElement;
    $('modal').classList.remove('hidden');
    var first = $('modalBody').querySelector('input, select, textarea');
    if (first && first.focus) first.focus();
    else if ($('modalSave').focus) $('modalSave').focus();
  }
  function closeModal() {
    $('modal').classList.add('hidden');
    state.modalMode = null;
    if (state.modalReturnFocus && state.modalReturnFocus.focus) state.modalReturnFocus.focus();
  }
  function modalHtml(title, fields) {
    return '<h2 id="modalTitle">' + esc(title) + '</h2>' + fields;
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
    openModal();
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
    openModal();
  }

  function modalSave() {
    if (state.modalMode === 'profile') {
      var name = $('mName').value.trim();
      if (!name) { toast(t('errNameRequired'), true); $('mName').focus(); return; }
      var p = state.modalProfile || { id:uid('p'), signals:[] };
      p.name = name;
      p.nameBn = $('mNameBn').value.trim() || (state.lang === 'bn' ? name : '');
      p.icon = $('mIcon').value; p.color = $('mColor').value;
      if (!state.modalProfile) state.db.profiles.push(p);
      saveDb().then(renderAll); closeModal(); return;
    }
    if (state.modalMode === 'signal') {
      var prof = currentProfile();
      if (!prof) { toast(t('noProfiles'), true); return; }
      var s = state.modalSignal || { id:uid('s') };
      var kind = $('mKind').value;
      s.label = $('mLabel').value.trim() || t('defaultButtonLabel');
      s.labelBn = $('mLabelBn').value.trim() || (state.lang === 'bn' ? s.label : '');
      s.icon = $('mIcon').value; s.color = $('mColor').value;
      s.kind = kind; s.repeat = Math.max(1, Math.min(10, parseInt($('mRepeat').value, 10) || 1));
      if (kind === 'protocol') {
        s.protocol = $('mProtocol').value.trim().toUpperCase() || 'NEC';
        var rawVal = $('mValue').value.trim();
        var num = (/^0x/i.test(rawVal) ? parseInt(rawVal, 16) : parseInt(rawVal, 10));
        if (isNaN(num) || num < 0) { toast(t('errHexValue'), true); $('mValue').focus(); return; }
        s.value = num; s.bits = Math.max(1, Math.min(64, parseInt($('mBits').value, 10) || 32));
        delete s.khz; delete s.timings;
      } else {
        s.khz = Math.max(30, Math.min(60, parseInt($('mKhz').value, 10) || 38));
        var arr = $('mTimings').value.split(/[\s,;]+/).filter(function (x) { return x !== ''; })
                    .map(function (x) { return parseInt(x, 10); })
                    .filter(function (x) { return !isNaN(x) && x >= 0 && x <= 65535; })
                    .slice(0, 128);
        if (arr.length < 4) { toast(t('errRawTimings'), true); $('mTimings').focus(); return; }
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

  function learnResultText(l) {
    var lines = [t('protocol') + ': ' + (l.protocol || t('unknownProtocol')),
                 t('hex') + ': ' + (l.hex || '—'),
                 t('bits') + ': ' + (l.bits || 0),
                 t('khz') + ': ' + (l.khz || 38)];
    if (l.timings && l.timings.length)
      lines.push(t('raw') + ':\n' + l.timings.join(', '));
    return lines.join('\n');
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
          $('learnResult').textContent = learnResultText(r.capture);
          $('learnSave').classList.remove('hidden');
        } else {
          $('learnResult').textContent = t('learnNoCapture');
          $('learnSave').classList.add('hidden');
        }
      }
    }).catch(function () {
      clearInterval(state.poll); state.poll = null;
      $('learnStart').classList.remove('hidden');
      $('learnCancel').classList.add('hidden');
      toast(t('statusLoadFailed'), true);
    });
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
      }).catch(function () { toast(t('statusLoadFailed'), true); }); }
    });

    document.addEventListener('keydown', function (e) {
      if (e.key === 'Escape' && !$('modal').classList.contains('hidden')) closeModal();
    });

    $('langBtn').addEventListener('click', function () {
      state.lang = state.lang === 'en' ? 'bn' : 'en';
      if (!state.cfg) state.cfg = {};
      state.cfg.locale = state.lang;
      rememberLang(state.lang);
      applyI18n(); renderAll();
      post('/api/config', { locale:state.lang })
        .catch(function () { toast(t('localeSaveFailed'), true); });
    });
    $('profileSelect').addEventListener('change', renderSignals);
    $('addProfileBtn').addEventListener('click', function () { openProfileEditor(null); });
    $('editProfileBtn').addEventListener('click', function () {
      var p = currentProfile(); if (p) openProfileEditor(p); else toast(t('noProfiles'), true);
    });
    $('dupProfileBtn').addEventListener('click', function () {
      var p = currentProfile(); if (!p) return;
      var c = JSON.parse(JSON.stringify(p));
      c.id = uid('p'); c.name = (c.name || '') + t('copySuffix');
      c.nameBn = (c.nameBn || '') + t('copySuffix');
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
    function restartDevice() {
      toast(t('restarting'));
      post('/api/restart', {}).catch(function () { /* the web server stops during reboot */ })
        .then(function () { setTimeout(function () { location.reload(); }, 1200); });
    }
    $('noticeRestartBtn').addEventListener('click', function () {
      if (!confirm(t('confirmRestart'))) return;
      restartDevice();
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
      }).catch(function () { toast(t('learnStartFailed'), true); });
    });
    $('learnCancel').addEventListener('click', function () {
      if (state.poll) { clearInterval(state.poll); state.poll = null; }
      post('/api/learn/cancel', {}).then(function () {
        $('learnStart').classList.remove('hidden');
        $('learnCancel').classList.add('hidden');
        $('learnResult').textContent = t('learnIdle');
      }).catch(function () {
        $('learnStart').classList.remove('hidden');
        $('learnCancel').classList.add('hidden');
        toast(t('learnCancelFailed'), true);
      });
    });
    $('learnSave').addEventListener('click', function () {
      if (!currentProfile()) { toast(t('noProfiles'), true); return; }
      var l = state.learned; if (!l) return;
      var s = l.kind === 'raw'
        ? { kind:'raw', protocol:'RAW', khz:l.khz, timings:l.timings, value:l.hex }
        : { kind:'protocol', protocol:l.protocol, value:l.hex, bits:l.bits };
      s.label = l.protocol || t('learnedButtonLabel');
      s.labelBn = state.lang === 'bn' ? s.label : '';
      s.icon = '⭐'; s.color = '#4f8cff'; s.repeat = 1;
      openSignalEditor(s);
      state.learned = null;
    });

    $('configForm').addEventListener('submit', function (ev) {
      ev.preventDefault();
      var deviceName = $('cfgDeviceName').value.trim();
      var apSsid = $('cfgApSsid').value.trim();
      var apPassword = $('cfgApPass').value;
      var wifiPassword = $('cfgWifiPass').value;
      var httpAuth = $('cfgAuth').checked;
      var httpUser = $('cfgUser').value.trim();
      var httpPassword = $('cfgHttpPass').value;
      if (!deviceName) { toast(t('errDeviceName'), true); $('cfgDeviceName').focus(); return; }
      if (!apSsid) { toast(t('errApSsid'), true); $('cfgApSsid').focus(); return; }
      if (apPassword && (apPassword.length < 8 || apPassword.length > 63)) {
        toast(t('errApPassword'), true); $('cfgApPass').focus(); return;
      }
      if (wifiPassword.length > 63) { toast(t('errWifiPassword'), true); $('cfgWifiPass').focus(); return; }
      if (httpAuth && !httpUser) { toast(t('errHttpUser'), true); $('cfgUser').focus(); return; }
      if (httpAuth && httpPassword.length < 4) {
        toast(t('errHttpPassword'), true); $('cfgHttpPass').focus(); return;
      }
      var c = {
        deviceName:deviceName,
        apSsid:apSsid, apPassword:apPassword,
        wifiSsid:$('cfgWifiSsid').value.trim(), wifiPassword:wifiPassword,
        captivePortal:$('cfgCaptive').checked, httpAuth:httpAuth,
        httpUser:httpUser, httpPassword:httpPassword,
        irRepeat:Math.max(1, Math.min(10, parseInt($('cfgRepeat').value, 10) || 1)),
        locale:$('cfgLocale').value
      };
      post('/api/config', c).then(function (r) {
        state.cfg = c; state.lang = c.locale === 'bn' ? 'bn' : 'en';
        rememberLang(state.lang);
        applyI18n(); toast(t('saved')); showRestartNotice(!!(r && r.restartRequired)); loadAll();
      }).catch(function () { toast(t('configSaveFailed'), true); });
    });

    $('exportBtn').addEventListener('click', function () {
      // The persisted format id stays "ihr-backup" so backups remain restorable
      // by older firmware; only the downloaded filename carries the new brand.
      try {
        var payload = { format:'ihr-backup', version:1, config:state.cfg, signals:state.db };
        var blob = new Blob([JSON.stringify(payload, null, 2)], {type:'application/json'});
        var a = document.createElement('a');
        a.href = URL.createObjectURL(blob);
        a.download = 'rovixane-ir-backup-' + new Date().toISOString().slice(0,10) + '.json';
        a.click(); setTimeout(function () { URL.revokeObjectURL(a.href); }, 1000);
        toast(t('exported'));
      } catch (e) { toast(t('backupExportFailed'), true); }
    });
    $('restoreBtn').addEventListener('click', function () { $('restoreFile').click(); });
    $('restoreFile').addEventListener('change', function () {
      var f = this.files && this.files[0]; if (!f) return;
      var reader = new FileReader();
      reader.onload = function () {
        var d;
        try { d = JSON.parse(reader.result); }
        catch (e) { toast(t('backupInvalid'), true); return; }
        if (!d || d.format !== 'ihr-backup' || !d.config || !d.signals) {
          toast(t('backupInvalid'), true); return;
        }
        var needsRestart = false;
        post('/api/config', d.config)
          .then(function (r) {
            needsRestart = !!(r && r.restartRequired);
            return api('/api/signals', { method:'PUT',
              headers:{'Content-Type':'application/json'}, body:JSON.stringify(d.signals) });
          })
          .then(function () { toast(t('restored')); showRestartNotice(needsRestart); loadAll(); })
          .catch(function () { toast(t('saveFailed'), true); });
      };
      reader.readAsText(f);
      this.value = '';
    });

    $('restartBtn').addEventListener('click', function () {
      if (!confirm(t('confirmRestart'))) return;
      restartDevice();
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
      api('/api/status').then(function (s) { state.status = s; renderStatus(); })
        .catch(function () { toast(t('statusLoadFailed'), true); });
    });
  }

  applyI18n();
  showRestartNotice(false);
  bind();
  loadAll();
  setInterval(function () { api('/api/status').then(function (s) {
    state.status = s; renderStatus();
  }).catch(function () {}); }, 5000);
})();
