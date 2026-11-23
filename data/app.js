/* ROVIXANE IR V2 — offline universal remote dashboard. No external dependencies. */
(function () {
  'use strict';

  // ---------------------------------------------------------------------
  // Translations: key -> [English, Bengali]. Arrays guarantee key parity.
  // ---------------------------------------------------------------------
  var I18N = {
    appName: ['ROVIXANE IR', 'ROVIXANE IR'],
    appTitle: ['ROVIXANE IR — Universal Infrared Remote', 'ROVIXANE IR — সার্বজনীন ইনফ্রারেড রিমোট'],
    metaDescription: ['Offline universal infrared remote for NodeMCU ESP8266.', 'NodeMCU ESP8266-এর জন্য অফলাইন সার্বজনীন ইনফ্রারেড রিমোট।'],
    connecting: ['Connecting…', 'সংযোগ হচ্ছে…'],
    online: ['Home Wi-Fi', 'বাড়ির Wi-Fi'],
    apMode: ['Setup AP', 'সেটআপ এপি'],

    homeEyebrow: ['Universal remote', 'সার্বজনীন রিমোট'],
    homeTitle: ['My remotes', 'আমার রিমোট'],
    addRemote: ['Add remote', 'রিমোট যোগ করুন'],
    search: ['Search', 'খুঁজুন'],
    searchPlaceholder: ['Search remotes and buttons', 'রিমোট ও বাটন খুঁজুন'],
    favorites: ['Favorites', 'প্রিয়'],
    recent: ['Recent', 'সাম্প্রতিক'],
    quickButtons: ['Quick buttons', 'দ্রুত বাটন'],
    allRemotes: ['All remotes', 'সব রিমোট'],
    emptyHome: ['No remotes yet. Add a TV, AC, fan, set-top box or custom remote to begin.', 'এখনো রিমোট নেই। শুরু করতে টিভি, এসি, ফ্যান, সেটটপ বক্স বা কাস্টম রিমোট যোগ করুন।'],
    emptySearch: ['No remote or button matches your search.', 'আপনার খোঁজার সাথে কোনো রিমোট বা বাটন মেলেনি।'],
    noProfiles: ['Create a remote first.', 'প্রথমে একটি রিমোট তৈরি করুন।'],
    category: ['Category', 'ধরন'],
    catTv: ['TV', 'টিভি'],
    catAc: ['Air conditioner', 'এয়ার কন্ডিশনার'],
    catFan: ['Fan', 'ফ্যান'],
    catStb: ['Set-top box', 'সেটটপ বক্স'],
    catCustom: ['Custom remote', 'কাস্টম রিমোট'],
    deviceCount: ['remotes', 'রিমোট'],
    mappedCount: ['mapped', 'ম্যাপ করা'],
    button: ['Button', 'বাটন'],
    rename: ['Rename', 'নাম বদলান'],
    duplicate: ['Duplicate', 'অনুলিপি'],
    delete: ['Delete', 'মুছুন'],
    exportProfile: ['Export remote', 'রিমোট রপ্তানি'],
    importProfile: ['Import remote', 'রিমোট আমদানি'],
    profileMenu: ['Remote options', 'রিমোটের অপশন'],
    editLayout: ['Edit buttons', 'বাটন সম্পাদনা'],
    done: ['Done', 'শেষ'],
    addButton: ['Add button', 'বাটন যোগ'],
    editButton: ['Edit button', 'বাটন সম্পাদনা'],
    deleteButton: ['Delete button', 'বাটন মুছুন'],
    moveUp: ['Move up', 'উপরে সরান'],
    moveDown: ['Move down', 'নিচে সরান'],
    confirmDeleteProfile: ['Delete this remote and all of its buttons?', 'এই রিমোট ও এর সব বাটন মুছে ফেলবেন?'],
    confirmDeleteSignal: ['Delete this button? The learned signal stays available for other buttons.', 'এই বাটনটি মুছবেন? শেখা সংকেত অন্য বাটনের জন্য থাকবে।'],
    confirmFactory: ['Factory reset erases every remote, button and setting. Type RESET to confirm.', 'ফ্যাক্টরি রিসেট সব রিমোট, বাটন ও সেটিংস মুছে ফেলবে। নিশ্চিত করতে RESET লিখুন।'],
    confirmRestart: ['Restart the device now?', 'এখন ডিভাইস রিস্টার্ট করবেন?'],
    factoryHint: ['Factory reset started. Reconnect to the ROVIXANE-IR-Setup Wi-Fi after reboot.', 'ফ্যাক্টরি রিসেট শুরু হয়েছে। রিস্টার্টের পর ROVIXANE-IR-Setup Wi-Fi-তে আবার সংযোগ করুন।'],
    restarting: ['Restarting…', 'রিস্টার্ট হচ্ছে…'],
    saved: ['Saved', 'সংরক্ষিত'],
    save: ['Save', 'সংরক্ষণ'],
    savedSettings: ['Settings saved', 'সেটিংস সংরক্ষিত'],
    sent: ['Signal sent', 'সংকেত পাঠানো হয়েছে'],
    sendFailed: ['Could not send the signal. Check the connection and wiring.', 'সংকেত পাঠানো যায়নি। সংযোগ ও ওয়্যারিং যাচাই করুন।'],
    saveFailed: ['Could not save. Check the connection and try again.', 'সংরক্ষণ করা যায়নি। সংযোগ যাচাই করে আবার চেষ্টা করুন।'],
    loadFailed: ['Could not load dashboard data. Check the connection and refresh.', 'ড্যাশবোর্ডের তথ্য লোড করা যায়নি। সংযোগ যাচাই করে রিফ্রেশ করুন।'],
    requestFailed: ['Request failed', 'অনুরোধ ব্যর্থ'],
    statusCode: ['Status code', 'স্টিতি কোড'],
    errBadRequest: ['The device rejected the values. Check the form.', 'ডিভাইস মানগুলো গ্রহণ করেনি। ফর্মটি যাচাই করুন।'],
    errUnauthorized: ['Login required. Check the username and password.', 'লগইন প্রয়োজন। ব্যবহারকারী ও পাসওয়ার্ড যাচাই করুন।'],
    errForbidden: ['Access denied.', 'প্রবেশ অনুমতি নেই।'],
    errNotFound: ['Not found.', 'পাওয়া যায়নি।'],
    errServer: ['The device returned a server error.', 'ডিভাইস সার্ভার ত্রুটি দিয়েছে।'],
    errNetwork: ['No response from the device. Check the connection.', 'ডিভাইস থেকে সাড়া নেই। সংযোগ যাচাই করুন।'],
    setupRequiredError: ['Create an admin password before changing settings.', 'সেটিংস বদলানোর আগে অ্যাডমিন পাসওয়ার্ড তৈরি করুন।'],
    csrfError: ['Security check failed. Reload the page and try again.', 'নিরাপত্তা যাচাই ব্যর্থ। পুনরায় লোড করে চেষ্টা করুন।'],
    contentTypeError: ['The request format was rejected.', 'অনুরোধের ফরম্যাট গ্রহণ করা হয়নি।'],
    storageError: ['The device could not write to storage. Existing data was kept.', 'ডিভাইস স্টোরেজে লিখতে পারেনি। আগের তথ্য সংরক্ষিত আছে।'],
    unsupportedError: ['This signal is not supported by the firmware build.', 'এই সংকেত ফার্মওয়্যার বিল্ডে সমর্থিত নয়।'],
    otaError: ['Firmware update failed.', 'ফার্মওয়্যার আপডেট ব্যর্থ হয়েছে।'],
    secretPlaceholder: ['Leave blank to keep current', 'অপরিবর্তিত রাখতে খালি রাখুন'],
    keepCurrent: ['Keep current', 'অপরিবর্তিত রাখুন'],

    setupTitle: ['First-time setup', 'প্রথমবারের সেটআপ'],
    setupStep1: ['Connect your phone to the ROVIXANE-IR-Setup Wi-Fi network.', 'ফোনকে ROVIXANE-IR-Setup Wi-Fi নেটওয়ার্কে সংযোগ করুন।'],
    setupStep2: ['Open http://192.168.4.1 in your browser.', 'ব্রাউজারে http://192.168.4.1 খুলুন।'],
    setupStep3: ['Create an admin password, then add your home Wi-Fi in Settings.', 'অ্যাডমিন পাসওয়ার্ড তৈরি করুন, তারপর সেটিংসে বাড়ির Wi-Fi যোগ করুন।'],
    openSettings: ['Open Settings', 'সেটিংস খুলুন'],
    setupRequiredTitle: ['Protect this device', 'ডিভাইসটি সুরক্ষিত করুন'],
    setupRequiredText: ['Create an admin password before configuring Wi-Fi or learning buttons. Passwords are never shown again.', 'Wi-Fi কনফিগার বা বাটন শেখার আগে অ্যাডমিন পাসওয়ার্ড তৈরি করুন। পাসওয়ার্ড আর কখনো দেখানো হয় না।'],
    createPassword: ['Create admin password', 'অ্যাডমিন পাসওয়ার্ড তৈরি করুন'],
    adminUser: ['Admin user', 'অ্যাডমিন ব্যবহারকারী'],
    newPassword: ['New password', 'নতুন পাসওয়ার্ড'],
    confirmPassword: ['Confirm password', 'পাসওয়ার্ড নিশ্চিত করুন'],
    errSetupPassword: ['Password must be 8–64 characters.', 'পাসওয়ার্ড ৮–৬৪ অক্ষরের হতে হবে।'],
    errPasswordMismatch: ['Passwords do not match.', 'পাসওয়ার্ড দুটি মেলেনি।'],
    setupComplete: ['Admin password created', 'অ্যাডমিন পাসওয়ার্ড তৈরি হয়েছে'],

    device: ['Device', 'ডিভাইস'],
    settingsTitle: ['Settings', 'সেটিংস'],
    deviceNameLbl: ['Device name', 'ডিভাইসের নাম'],
    language: ['Language', 'ভাষা'],
    theme: ['Theme', 'থিম'],
    themeDark: ['Dark', 'ডার্ক'],
    themeLight: ['Light', 'লাইট'],
    network: ['Network', 'নেটওয়ার্ক'],
    networkTitle: ['Wi-Fi', 'Wi-Fi'],
    apSsidLbl: ['Setup AP name', 'সেটআপ এপির নাম'],
    apPassLbl: ['Setup AP password', 'সেটআপ এপির পাসওয়ার্ড'],
    openAp: ['Open setup AP (no WPA password)', 'উন্মুক্ত সেটআপ এপি (WPA পাসওয়ার্ড ছাড়া)'],
    wifiSsidLbl: ['Home Wi-Fi name', 'বাড়ির Wi-Fi নাম'],
    wifiPassLbl: ['Home Wi-Fi password', 'বাড়ির Wi-Fi পাসওয়ার্ড'],
    captiveLbl: ['Captive portal', 'ক্যাপ্টিভ পোর্টাল'],
    security: ['Security', 'নিরাপত্তা'],
    securityTitle: ['Dashboard login', 'ড্যাশবোর্ড লগইন'],
    httpAuthLbl: ['Require dashboard login', 'ড্যাশবোর্ডে লগইন প্রয়োজন'],
    httpUserLbl: ['Login user', 'লগইন ব্যবহারকারী'],
    httpPassLbl: ['New login password', 'নতুন লগইন পাসওয়ার্ড'],
    passwordNote: ['Passwords are masked in all API responses and backups. Leave a password field blank to keep the current one.', 'সব API উত্তর ও ব্যাকআপে পাসওয়ার্ড মাস্ক করা থাকে। বর্তমান পাসওয়ার্ড রাখতে ঘরটি খালি রাখুন।'],
    ir: ['Infrared', 'ইনফ্রারেড'],
    irTitle: ['IR behaviour', 'IR আচরণ'],
    repeatLbl: ['Default repeats (1–10)', 'ডিফল্ট পুনরাবৃত্তি (১–১০)'],
    saveSettings: ['Save settings', 'সেটিংস সংরক্ষণ'],
    restartNotice: ['Saved. Restart to apply network or login changes.', 'সংরক্ষিত। নেটওয়ার্ক বা লগইন পরিবর্তন প্রয়োগ করতে রিস্টার্ট করুন।'],
    restart: ['Restart', 'রিস্টার্ট'],
    errDeviceName: ['Enter a device name.', 'ডিভাইসের নাম দিন।'],
    errApSsid: ['Enter a setup AP name.', 'সেটআপ এপির নাম দিন।'],
    errApPassword: ['AP password must be empty or 8–63 characters.', 'এপি পাসওয়ার্ড খালি বা ৮–৬৩ অক্ষরের হতে হবে।'],
    errWifiPassword: ['Wi-Fi password must be 64 characters or fewer.', 'Wi-Fi পাসওয়ার্ড সর্বোচ্চ ৬৪ অক্ষর।'],
    errHttpUser: ['Enter a login user when login is enabled.', 'লগইন চালু থাকলে ব্যবহারকারীর নাম দিন।'],
    errHttpPassword: ['Login password must be at least 8 characters.', 'লগইন পাসওয়ার্ড কমপক্ষে ৮ অক্ষরের হতে হবে।'],
    configSaveFailed: ['Settings were not saved. Check the values and try again.', 'সেটিংস সংরক্ষণ হয়নি। মান যাচাই করে আবার চেষ্টা করুন।'],

    firmware: ['Firmware', 'ফার্মওয়্যার'],
    otaTitle: ['Firmware update', 'ফার্মওয়্যার আপডেট'],
    otaHelp: ['Upload a compiled .bin from your phone. The image is streamed to the OTA slot and verified before restart. Keep the device powered during the update.', 'ফোন থেকে কম্পাইল করা .bin আপলোড করুন। ইমেজটি OTA স্লটে স্ট্রিম হয়ে যাচাইয়ের পর রিস্টার্ট হবে। আপডেটের সময় বিদ্যুৎ সংযোগ রাখুন।'],
    currentVersion: ['Current version', 'বর্তমান সংস্করণ'],
    otaSupport: ['OTA support', 'OTA সমর্থন'],
    chooseFirmware: ['Firmware file (.bin)', 'ফার্মওয়্যার ফাইল (.bin)'],
    uploadFirmware: ['Upload firmware', 'ফার্মওয়্যার আপলোড'],
    otaReady: ['Ready to upload.', 'আপলোডের জন্য প্রস্তুত।'],
    otaUploading: ['Uploading firmware… do not close this page.', 'ফার্মওয়্যার আপলোড হচ্ছে… পৃষ্ঠাটি বন্ধ করবেন না।'],
    otaSuccess: ['Firmware verified. The device is restarting.', 'ফার্মওয়্যার যাচাই হয়েছে। ডিভাইস রিস্টার্ট হচ্ছে।'],
    otaFailed: ['Update failed. The previous firmware is still active.', 'আপডেট ব্যর্থ। আগের ফার্মওয়্যার সক্রিয় আছে।'],
    otaRecovery: ['If the device does not return, power-cycle it and use USB flashing with the published BIN. OTA cannot recover a device with a damaged bootloader.', 'ডিভাইস ফিরে না এলে পাওয়ার সাইকেল করে প্রকাশিত BIN দিয়ে USB ফ্ল্যাশ করুন। বুটলোডার ক্ষতিগ্রস্ত হলে OTA তা পুনরুদ্ধার করতে পারে না।'],
    otaConfirm: ['Upload this firmware and restart the device?', 'এই ফার্মওয়্যার আপলোড করে ডিভাইস রিস্টার্ট করবেন?'],
    errFileType: ['Choose a .bin firmware file.', '.bin ফার্মওয়্যার ফাইল বাছুন।'],
    errFileSize: ['File size is outside the allowed range.', 'ফাইলের আকার অনুমোদিত সীমার বাইরে।'],
    errOtaUnsupported: ['This flash layout has no usable OTA slot. Reflash with a 4MB/1MB OTA layout.', 'এই ফ্ল্যাশ লেআউটে OTA স্লট নেই। 4MB/1MB OTA লেআউটে রিফ্ল্যাশ করুন।'],
    noFile: ['No file selected', 'কোনো ফাইল বাছা হয়নি'],

    data: ['Data', 'তথ্য'],
    dataTitle: ['Backup & restore', 'ব্যাকআপ ও পুনরুদ্ধার'],
    backupHint: ['Backups include remotes, button mappings and settings. Passwords are intentionally excluded.', 'ব্যাকআপে রিমোট, বাটন ম্যাপিং ও সেটিংস থাকে। পাসওয়ার্ড ইচ্ছাকৃতভাবে বাদ।'],
    exportJson: ['Download backup', 'ব্যাকআপ ডাউনলোড'],
    restoreJson: ['Restore backup', 'ব্যাকআপ পুনরুদ্ধার'],
    backupInvalid: ['This file is not a valid ROVIXANE IR backup.', 'ফাইলটি বৈধ ROVIXANE IR ব্যাকআপ নয়।'],
    exported: ['Backup downloaded', 'ব্যাকআপ ডাউনলোড হয়েছে'],
    restored: ['Backup restored', 'ব্যাকআপ পুনরুদ্ধার হয়েছে'],
    profileImported: ['Remote imported', 'রিমোট আমদানি হয়েছে'],
    profileImportInvalid: ['This file is not a valid remote export.', 'ফাইলটি বৈধ রিমোট রপ্তানি নয়।'],
    reset: ['Reset', 'রিসেট'],
    resetTitle: ['Restart & reset', 'রিস্টার্ট ও রিসেট'],
    factoryReset: ['Factory reset', 'ফ্যাক্টরি রিসেট'],
    factoryWarn: ['Factory reset erases every remote, button and setting. It cannot be undone.', 'ফ্যাক্টরি রিসেট সব রিমোট, বাটন ও সেটিংস মুছে ফেলে। এটি ফেরানো যায় না।'],
    about: ['About', 'সম্পর্কে'],
    diagTitle: ['Diagnostics', 'ডায়াগনস্টিকস'],
    loading: ['Loading…', 'লোড হচ্ছে…'],
    refresh: ['Refresh', 'রিফ্রেশ'],
    navHome: ['Home', 'হোম'],
    navLearn: ['Learn', 'শেখা'],
    navSettings: ['Settings', 'সেটিংস'],

    advanced: ['Advanced', 'উন্নত'],
    learnTitle: ['Learn a signal', 'সংকেত শেখা'],
    learnHelp: ['Point your remote at the TSOP38238 receiver and press a button. Unknown protocols are preserved as raw timing data.', 'রিমোটটি TSOP38238 রিসিভারের দিকে তাক করে বাটন চাপুন। অজানা প্রোটোকল raw টাইমিং হিসেবে সংরক্ষিত হয়।'],
    startLearning: ['Start learning', 'শেখা শুরু'],
    cancel: ['Cancel', 'বাতিল'],
    learning: ['Waiting for a signal…', 'সংকেতের অপেক্ষায়…'],
    saveAsButton: ['Save as button', 'বাটন হিসেবে সংরক্ষণ'],
    howToLearn: ['How per-button learning works', 'বাটনভিত্তিক শেখা কীভাবে কাজ করে'],
    howToLearnText: ['Open a remote, tap an unconfigured button, then press the matching button on your physical remote. You can re-learn a mapped button from its edit sheet.', 'রিমোট খুলে অকনফিগার করা বাটনে চাপ দিন, তারপর আসল রিমোটের সংশ্লিষ্ট বাটন চাপুন। ম্যাপ করা বাটন সম্পাদনা শিট থেকে আবার শেখা যায়।'],
    learnForButton: ['Learn this button', 'এই বাটন শেখান'],
    learnWaiting: ['Press the matching button on your physical remote.', 'আসল রিমোটের সংশ্লিষ্ট বাটন চাপুন।'],
    learnReady: ['Signal captured', 'সংকেত পাওয়া গেছে'],
    learnInvalid: ['No usable signal was captured. Try again closer to the receiver.', 'ব্যবহারযোগ্য সংকেত পাওয়া যায়নি। রিসিভারের কাছে গিয়ে আবার চেষ্টা করুন।'],
    learnOverflow: ['The capture buffer overflowed. Try again closer to the receiver.', 'ক্যাপচার বাফার উপচে পড়েছে। রিসিভারের কাছে গিয়ে আবার চেষ্টা করুন।'],
    learnUnsupported: ['The decoded protocol is not enabled for sending; the raw capture will be saved instead.', 'ডিকোড করা প্রোটোকল পাঠানোর জন্য চালু নেই; পরিবর্তে raw ক্যাপচার সংরক্ষিত হবে।'],
    learnRetry: ['Try again', 'আবার চেষ্টা'],
    saveSignal: ['Save signal', 'সংকেত সংরক্ষণ'],
    signalSaved: ['Button learned', 'বাটন শেখা হয়েছে'],
    confirmOverwrite: ['This button already has a signal. Replace it?', 'এই বাটনে আগে থেকেই সংকেত আছে। প্রতিস্থাপন করবেন?'],
    notConfigured: ['Not configured — tap to learn', 'কনফিগার করা নেই — শিখতে চাপ দিন'],
    unverifiedCode: ['Example code — verify it with your own device before relying on it.', 'উদাহরণ কোড — নির্ভর করার আগে নিজের ডিভাইসে যাচাই করুন।'],
    relearn: ['Re-learn', 'আবার শেখান'],
    learn: ['Learn', 'শেখান'],
    protocol: ['Protocol', 'প্রোটোকল'],
    hex: ['HEX value', 'HEX মান'],
    bits: ['Bit length', 'বিট সংখ্যা'],
    raw: ['Raw timings', 'Raw টাইমিং'],
    khz: ['Carrier (kHz)', 'ক্যারিয়ার (kHz)'],
    signalType: ['Signal type', 'সংকেতের ধরন'],
    timings: ['Timings', 'টাইমিং'],
    repeat: ['Repeat', 'পুনরাবৃত্তি'],

    deviceNameMeta: ['Device', 'ডিভাইস'],
    remoteOptions: ['Options', 'অপশন'],
    profileName: ['Remote name', 'রিমোটের নাম'],
    profileNameBn: ['Remote name (Bangla)', 'রিমোটের নাম (বাংলা)'],
    profileIcon: ['Icon', 'আইকন'],
    profileColor: ['Color', 'রঙ'],
    profileCategory: ['Remote type', 'রিমোটের ধরন'],
    createRemote: ['Create remote', 'রিমোট তৈরি'],
    addRemoteTitle: ['Add a remote', 'রিমোট যোগ করুন'],
    addRemoteHelp: ['Choose a template. Every button starts unconfigured until you learn it from the physical remote.', 'একটি টেমপ্লেট বাছুন। আসল রিমোট থেকে না শেখা পর্যন্ত প্রতিটি বাটন অকনফিগার থাকে।'],
    name: ['Name', 'নাম'],
    nameBn: ['Name (Bangla)', 'নাম (বাংলা)'],
    icon: ['Icon', 'আইকন'],
    color: ['Color', 'রঙ'],
    width: ['Width', 'প্রস্থ'],
    favorite: ['Favorite', 'প্রিয়'],
    assignSignal: ['Assigned signal', 'নির্ধারিত সংকেত'],
    notMapped: ['Not mapped', 'ম্যাপ করা নেই'],
    learnNew: ['Learn a new signal', 'নতুন সংকেত শেখান'],
    errNameRequired: ['Enter a name before saving.', 'সংরক্ষণের আগে নাম দিন।'],
    tapToLearn: ['Tap to learn this button', 'এই বাটন শিখতে চাপ দিন'],
    longPressHint: ['Long-press a mapped button to re-learn it.', 'ম্যাপ করা বাটনে লম্বা চাপ দিয়ে আবার শেখান।'],
    editMode: ['Editing buttons', 'বাটন সম্পাদনা চলছে'],
    favoritesHint: ['Star a remote to pin it here.', 'রিমোটে তারা চিহ্ন দিয়ে এখানে পিন করুন।'],

    temperature: ['Temperature', 'তাপমাত্রা'],
    mode: ['Mode', 'মোড'],
    fanSpeed: ['Fan', 'ফ্যান'],
    acNote: ['State display only. Every AC command must be learned from your own remote; unsupported protocols are never faked.', 'শুধু অবস্থা প্রদর্শন। প্রতিটি এসি কমান্ড নিজের রিমোট থেকে শিখতে হবে; অসমর্থিত প্রোটোকল নকল করা হয় না।'],
    acPower: ['Power', 'পাওয়ার'],

    btnPower: ['Power', 'পাওয়ার'], btnSource: ['Source', 'উৎস'], btnMute: ['Mute', 'মিউট'],
    btnUp: ['Up', 'উপরে'], btnDown: ['Down', 'নিচে'], btnLeft: ['Left', 'বাম'], btnRight: ['Right', 'ডান'],
    btnOk: ['OK', 'ঠিক আছে'], btnBack: ['Back', 'ফিরে'], btnHome: ['Home', 'হোম'], btnMenu: ['Menu', 'মেনু'],
    btnVolUp: ['Vol +', 'ভলিউম +'], btnVolDown: ['Vol −', 'ভলিউম −'],
    btnChUp: ['Ch +', 'চ্যানেল +'], btnChDown: ['Ch −', 'চ্যানেল −'],
    btnPlay: ['Play', 'প্লে'], btnPause: ['Pause', 'পজ'], btnRew: ['Rewind', 'পেছনে'], btnFwd: ['Forward', 'সামনে'],
    btnInfo: ['Info', 'তথ্য'], btnExit: ['Exit', 'প্রস্থান'], btnGuide: ['Guide', 'গাইড'],
    btnSpeed: ['Speed', 'গতি'], btnMode: ['Mode', 'মোড'], btnOsc: ['Oscillate', 'দোলন'], btnTimer: ['Timer', 'টাইমার'], btnLight: ['Light', 'লাইট'],
    btnSwing: ['Swing', 'সুইং'], btnTurbo: ['Turbo', 'টার্বো'], btnEco: ['Eco', 'ইকো'], btnSleep: ['Sleep', 'স্লিপ'],
    btnTempUp: ['Temp +', 'তাপমাত্রা +'], btnTempDown: ['Temp −', 'তাপমাত্রা −'],
    btnFan: ['Fan', 'ফ্যান'], btnCool: ['Cool', 'কুল'], btnHeat: ['Heat', 'হিট'], btnDry: ['Dry', 'ড্রাই'], btnAuto: ['Auto', 'অটো'],
    num0: ['0', '০'], num1: ['1', '১'], num2: ['2', '২'], num3: ['3', '৩'], num4: ['4', '৪'],
    num5: ['5', '৫'], num6: ['6', '৬'], num7: ['7', '৭'], num8: ['8', '৮'], num9: ['9', '৯'],

    secPower: ['Power', 'পাওয়ার'], secNavigation: ['Navigation', 'নেভিগেশন'], secVolume: ['Volume & channel', 'ভলিউম ও চ্যানেল'],
    secNumbers: ['Numbers', 'সংখ্যা'], secMedia: ['Media', 'মিডিয়া'], secFan: ['Fan controls', 'ফ্যান নিয়ন্ত্রণ'],
    secAc: ['Air conditioner', 'এয়ার কন্ডিশনার'], secCustom: ['Custom buttons', 'কাস্টম বাটন'],

    modeCool: ['Cool', 'কুল'], modeHeat: ['Heat', 'হিট'], modeDry: ['Dry', 'ড্রাই'], modeFan: ['Fan', 'ফ্যান'], modeAuto: ['Auto', 'অটো'],
    fanAuto: ['Auto', 'অটো'], fanLow: ['Low', 'কম'], fanMid: ['Medium', 'মাঝারি'], fanHigh: ['High', 'বেশি'],

    diagFirmware: ['Firmware', 'ফার্মওয়্যার'], diagUptime: ['Uptime (s)', 'চলার সময় (সে)'],
    diagHeap: ['Free heap (B)', 'মুক্ত হিপ (B)'], diagFs: ['Storage used / total (B)', 'স্টোরেজ ব্যবহৃত / মোট (B)'],
    diagIp: ['IP address', 'IP ঠিকানা'], diagWifi: ['Wi-Fi state', 'Wi-Fi অবস্থা'], diagRssi: ['RSSI (dBm)', 'RSSI (dBm)'],
    diagReset: ['Reset reason', 'রিসেটের কারণ'], diagLocale: ['Language', 'ভাষা'],
    diagSketch: ['Sketch / OTA free (B)', 'স্কেচ / OTA খালি (B)'], diagSignals: ['Profiles / signals', 'প্রোফাইল / সংকেত'],
    diagSetup: ['Setup required', 'সেটআপ প্রয়োজন'], diagBuild: ['Build', 'বিল্ড'],
    yes: ['Yes', 'হ্যাঁ'], no: ['No', 'না'], staMode: ['Home Wi-Fi', 'বাড়ির Wi-Fi'],
    resetPower: ['Power on', 'পাওয়ার চালু'], resetExternal: ['External reset', 'বাহ্যিক রিসেট'],
    resetWatchdog: ['Watchdog reset', 'ওয়াচডগ রিসেট'], resetDeepSleep: ['Deep-sleep wake', 'ডিপ-স্লিপ জাগরণ'],
    resetSoftware: ['Software restart', 'সফটওয়্যার রিস্টার্ট'], resetUnknown: ['Unknown', 'অজানা'],

    ariaTheme: ['Toggle theme', 'থিম বদলান'], ariaLanguage: ['Switch language', 'ভাষা বদলান'],
    ariaMainNav: ['Main navigation', 'প্রধান নেভিগেশন'], ariaBack: ['Back', 'ফিরে যান'],
    ariaFavorite: ['Favorite', 'প্রিয়'], ariaMore: ['More options', 'আরও অপশন'],
    aboutText: ['ROVIXANE IR V2 — offline universal IR remote for ESP8266.', 'ROVIXANE IR V2 — ESP8266-এর জন্য অফলাইন সার্বজনীন IR রিমোট।']
  };

  var BTN = {
    power:{label:'btnPower',icon:'⏻',tone:'power'},
    source:{label:'btnSource',icon:'⌘'},
    mute:{label:'btnMute',icon:'🔇'},
    up:{label:'btnUp',icon:'▲'}, down:{label:'btnDown',icon:'▼'},
    left:{label:'btnLeft',icon:'◀'}, right:{label:'btnRight',icon:'▶'},
    ok:{label:'btnOk',icon:'●'}, back:{label:'btnBack',icon:'↩'},
    home:{label:'btnHome',icon:'⌂'}, menu:{label:'btnMenu',icon:'☰'},
    volup:{label:'btnVolUp',icon:'🔊'}, voldown:{label:'btnVolDown',icon:'🔈'},
    chup:{label:'btnChUp',icon:'＋'}, chdown:{label:'btnChDown',icon:'−'},
    play:{label:'btnPlay',icon:'▶'}, pause:{label:'btnPause',icon:'⏸'},
    rew:{label:'btnRew',icon:'⏮'}, fwd:{label:'btnFwd',icon:'⏭'},
    info:{label:'btnInfo',icon:'ℹ'}, exit:{label:'btnExit',icon:'⏏'}, guide:{label:'btnGuide',icon:'☷'},
    speed:{label:'btnSpeed',icon:'🌀'}, mode:{label:'btnMode',icon:'◈'}, osc:{label:'btnOsc',icon:'↔'},
    timer:{label:'btnTimer',icon:'⏱'}, light:{label:'btnLight',icon:'💡'},
    swing:{label:'btnSwing',icon:'↕'}, turbo:{label:'btnTurbo',icon:'⚡'}, eco:{label:'btnEco',icon:'🍃'}, sleep:{label:'btnSleep',icon:'☾'},
    tempup:{label:'btnTempUp',icon:'＋'}, tempdown:{label:'btnTempDown',icon:'−'},
    fan:{label:'btnFan',icon:'🌀'}, cool:{label:'btnCool',icon:'❄'}, heat:{label:'btnHeat',icon:'☀'}, dry:{label:'btnDry',icon:'💧'}, auto:{label:'btnAuto',icon:'A'},
    n0:{label:'num0',icon:'0'}, n1:{label:'num1',icon:'1'}, n2:{label:'num2',icon:'2'}, n3:{label:'num3',icon:'3'},
    n4:{label:'num4',icon:'4'}, n5:{label:'num5',icon:'5'}, n6:{label:'num6',icon:'6'}, n7:{label:'num7',icon:'7'},
    n8:{label:'num8',icon:'8'}, n9:{label:'num9',icon:'9'}
  };

  var LAYOUTS = {
    tv: [
      {section:'power', keys:['power','source','mute']},
      {section:'navigation', dpad:true, keys:['up','left','ok','right','down'], extra:['back','home','menu']},
      {section:'volume', keys:['volup','voldown','chup','chdown']},
      {section:'numbers', keys:['n1','n2','n3','n4','n5','n6','n7','n8','n9','n0','info','exit']},
      {section:'media', keys:['rew','play','pause','fwd']}
    ],
    stb: [
      {section:'power', keys:['power','guide','mute']},
      {section:'navigation', dpad:true, keys:['up','left','ok','right','down'], extra:['back','home','menu']},
      {section:'volume', keys:['volup','voldown','chup','chdown']},
      {section:'numbers', keys:['n1','n2','n3','n4','n5','n6','n7','n8','n9','n0','info','exit']},
      {section:'media', keys:['rew','play','pause','fwd']}
    ],
    fan: [
      {section:'power', keys:['power']},
      {section:'fan', keys:['speed','mode','osc','timer','light']}
    ],
    ac: [
      {section:'power', keys:['power']},
      {section:'ac', keys:['tempdown','tempup','mode','fan','swing','turbo','eco','sleep','timer']}
    ],
    custom: []
  };

  var CATEGORIES = [
    {id:'tv', label:'catTv', icon:'📺', color:'#4f8cff'},
    {id:'ac', label:'catAc', icon:'❄', color:'#00c2c7'},
    {id:'fan', label:'catFan', icon:'🌀', color:'#37d67a'},
    {id:'stb', label:'catStb', icon:'📡', color:'#ffc857'},
    {id:'custom', label:'catCustom', icon:'✦', color:'#b06dff'}
  ];
  var ICONS = ['⏻','📺','🔊','🔈','🔇','➕','➖','⬆','⬇','⬅','➡','✅','↩','🏠','☰','▶','⏸','⏹','⏮','⏭','❄','☀','💧','🌀','⚡','🍃','☾','💡','⏱','⭐','✦','📡'];
  var COLORS = ['#4f8cff','#ff6b6b','#37d67a','#ffc857','#b06dff','#00c2c7','#e8eefc'];
  var ALIASES = {
    power:['power','on/off','পাওয়ার'], source:['source','input','উৎস'], mute:['mute','মিউট'],
    volup:['volume +','vol+','volume up','ভলিউম বাড়ান'], voldown:['volume -','volume −','vol-','volume down','ভলিউম কমান'],
    chup:['channel +','ch+','channel up','চ্যানেল +'], chdown:['channel -','channel −','ch-','channel down','চ্যানেল −'],
    up:['up','উপরে'], down:['down','নিচে'], left:['left','বাম'], right:['right','ডান'],
    ok:['ok','enter','ঠিক'], back:['back','return','ফিরে'], home:['home','হোম'], menu:['menu','মেনু'],
    play:['play','প্লে'], pause:['pause','পজ'], info:['info','তথ্য'], exit:['exit','প্রস্থান'], guide:['guide','গাইড'],
    speed:['speed','গতি'], mode:['mode','মোড'], osc:['osc','swing','দোলন'], timer:['timer','টাইমার'], light:['light','লাইট'],
    tempup:['temp +','temperature +','তাপমাত্রা +'], tempdown:['temp -','temp −','temperature -','তাপমাত্রা −'],
    swing:['swing','সুইং'], turbo:['turbo','টার্বো'], eco:['eco','ইকো'], sleep:['sleep','স্লিপ']
  };

  var state = {
    cfg:null, db:null, status:null, lang:'en', theme:'dark', view:'home',
    currentProfileId:null, editing:false, search:'', learned:null, advLearned:null,
    poll:null, learnTimer:null, sheetOnSave:null, sheetReturnFocus:null,
    ac:{}, otaFile:null, uploading:false
  };

  function $(id) { return document.getElementById(id); }
  function t(key) {
    var v = I18N[key];
    if (!v) return key;
    return state.lang === 'bn' ? v[1] : v[0];
  }
  function esc(s) {
    return String(s == null ? '' : s).replace(/[&<>"']/g, function (c) {
      return {'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c];
    });
  }
  function uid(prefix) {
    return prefix + '_' + Date.now().toString(36) + Math.floor(Math.random() * 1679616).toString(36);
  }
  function clone(o) { return JSON.parse(JSON.stringify(o)); }
  function text(obj, base) {
    if (!obj) return '';
    var bn = obj[base + 'Bn'];
    return (state.lang === 'bn' && bn) ? bn : (obj[base] || '');
  }

  // ---------------------------------------------------------------------
  // Networking
  // ---------------------------------------------------------------------
  function authToken() {
    try { return sessionStorage.getItem('rovixane-auth'); } catch (e) { return null; }
  }
  function rememberAuth(user, pass) {
    try {
      sessionStorage.setItem('rovixane-auth', btoa(user + ':' + pass));
    } catch (e) { /* storage unavailable */ }
  }
  function forgetAuth() {
    try { sessionStorage.removeItem('rovixane-auth'); } catch (e) { /* ignore */ }
  }

  function statusMessage(status) {
    var suffix = ' (' + t('statusCode') + ': ' + status + ')';
    if (status === 400) return t('errBadRequest') + suffix;
    if (status === 401) return t('errUnauthorized') + suffix;
    if (status === 403) return t('errForbidden') + suffix;
    if (status === 404) return t('errNotFound') + suffix;
    if (status >= 500) return t('errServer') + suffix;
    return t('requestFailed') + suffix;
  }
  function codeMessage(code, fallback) {
    if (code === 'setup_required') return t('setupRequiredError');
    if (code === 'csrf') return t('csrfError');
    if (code === 'content_type') return t('contentTypeError');
    if (code === 'storage') return t('storageError');
    if (code === 'unsupported') return t('unsupportedError');
    if (code === 'ota') return t('otaError');
    return fallback;
  }

  function api(path, opts) {
    opts = opts || {};
    var headers = opts.headers || {};
    headers['X-ROVIXANE-CSRF'] = '1';
    var token = authToken();
    if (token) headers['Authorization'] = 'Basic ' + token;
    if (opts.body && !headers['Content-Type']) headers['Content-Type'] = 'application/json';
    opts.headers = headers;
    return fetch(path, opts).then(function (r) {
      var ct = r.headers.get('content-type') || '';
      var parsed = ct.indexOf('json') >= 0 ? r.json() : r.text();
      return parsed.then(function (data) {
        if (!r.ok) {
          if (r.status === 401) forgetAuth();
          var fallback = statusMessage(r.status);
          var msg = (data && data.error) ? codeMessage(data.code, data.error) : fallback;
          var err = new Error(msg);
          err.status = r.status;
          err.code = data && data.code;
          throw err;
        }
        return data;
      });
    }).catch(function (e) {
      if (e && e.status) throw e;
      var err = new Error(t('errNetwork'));
      err.status = 0;
      throw err;
    });
  }
  function post(path, body) {
    return api(path, { method:'POST', body: JSON.stringify(body || {}) });
  }
  function put(path, body) {
    return api(path, { method:'PUT', body: JSON.stringify(body || {}) });
  }

  var toastTimer = null;
  function toast(message, isError) {
    var el = $('toast');
    el.textContent = message;
    el.className = 'toast' + (isError ? ' err' : '') + ' show';
    if (toastTimer) clearTimeout(toastTimer);
    toastTimer = setTimeout(function () { el.className = 'toast hidden'; }, 3400);
  }

  function loadAll() {
    return Promise.all([api('/api/config'), api('/api/signals'), api('/api/status')])
      .then(function (r) {
        state.cfg = r[0] || {};
        state.db = r[1];
        state.status = r[2] || {};
        if (!state.db || !Array.isArray(state.db.profiles)) state.db = { format:'ihr-signals', version:2, profiles:[] };
        state.lang = state.cfg.locale === 'bn' ? 'bn' : 'en';
        var generated = false;
        (state.db.profiles || []).forEach(function (p) {
          var hadButtons = Array.isArray(p.buttons) && p.buttons.length > 0;
          ensureLayout(p);
          if (!hadButtons && p.category !== 'custom' && p.buttons.length) generated = true;
        });
        applyI18n();
        fillConfig();
        renderAll();
        if (state.cfg.setupRequired) showSetup(true);
        // Persist the template layout on first use so button ids stay stable.
        if (generated && !state.cfg.setupRequired) saveDb(true).catch(function () {});
      })
      .catch(function (e) {
        toast(e.message || t('loadFailed'), true);
      });
  }

  function saveDb(silent) {
    return put('/api/signals', state.db).then(function () {
      if (!silent) toast(t('saved'));
    }).catch(function (e) {
      if (!silent) toast(e.message || t('saveFailed'), true);
      throw e;
    });
  }

  // ---------------------------------------------------------------------
  // Remote layouts and mapping
  // ---------------------------------------------------------------------
  function categoryMeta(id) {
    for (var i = 0; i < CATEGORIES.length; i++) if (CATEGORIES[i].id === id) return CATEGORIES[i];
    return CATEGORIES[4];
  }
  function catLabel(id) { return t(categoryMeta(id).label); }
  function bdef(key) { return BTN[key] || { label:key, icon:'⭐' }; }

  function ensureLayout(p) {
    if (!Array.isArray(p.buttons)) p.buttons = [];
    if (!p.buttons.length && p.category !== 'custom') {
      var layout = LAYOUTS[p.category] || [];
      layout.forEach(function (sec) {
        sec.keys.forEach(function (key) {
          var d = bdef(key);
          p.buttons.push({
            id: uid('b'), key: key, section: sec.section,
            label: d.label, icon: d.icon, color: '',
            w: key === 'ok' ? 1 : 1, favorite: false
          });
        });
        (sec.extra || []).forEach(function (key) {
          var d = bdef(key);
          p.buttons.push({
            id: uid('b'), key: key, section: sec.section,
            label: d.label, icon: d.icon, color: '',
            w: 1, favorite: false
          });
        });
      });
    }
    autoMap(p);
  }

  function autoMap(p) {
    if (!Array.isArray(p.signals)) p.signals = [];
    p.buttons.forEach(function (b) {
      if (b.signalId && signalFor(p, b)) return;
      var aliases = ALIASES[b.key] || [];
      if (!aliases.length) return;
      for (var i = 0; i < p.signals.length; i++) {
        var s = p.signals[i];
        var hay = ((s.label || '') + ' ' + (s.labelBn || '')).toLowerCase();
        for (var a = 0; a < aliases.length; a++) {
          if (hay.indexOf(aliases[a]) >= 0) { b.signalId = s.id; return; }
        }
      }
    });
  }

  function signalFor(p, b) {
    if (!b || !b.signalId || !Array.isArray(p.signals)) return null;
    for (var i = 0; i < p.signals.length; i++) if (p.signals[i].id === b.signalId) return p.signals[i];
    return null;
  }
  function buttonLabel(b) {
    if (state.lang === 'bn' && b.labelBn) return b.labelBn;
    if (b.key && BTN[b.key] && (!b.label || b.label === BTN[b.key].label)) return t(BTN[b.key].label);
    return b.label || (b.key ? t(bdef(b.key).label) : t('button'));
  }
  function buttonIcon(b) { return b.icon || bdef(b.key).icon; }

  // ---------------------------------------------------------------------
  // Home
  // ---------------------------------------------------------------------
  function recents() {
    try { return JSON.parse(localStorage.getItem('rovixane-recent') || '[]'); } catch (e) { return []; }
  }
  function rememberRecent(id) {
    var list = recents().filter(function (x) { return x !== id; });
    list.unshift(id);
    try { localStorage.setItem('rovixane-recent', JSON.stringify(list.slice(0, 8))); } catch (e) { /* ignore */ }
  }

  function profileMatches(p, q) {
    if (!q) return true;
    var hay = ((text(p, 'name') || p.name || '') + ' ' + catLabel(p.category)).toLowerCase();
    if (hay.indexOf(q) >= 0) return true;
    ensureLayout(p);
    return p.buttons.some(function (b) { return buttonLabel(b).toLowerCase().indexOf(q) >= 0; });
  }

  function deviceCard(p) {
    ensureLayout(p);
    var meta = categoryMeta(p.category);
    var mapped = p.buttons.filter(function (b) { return !!signalFor(p, b); }).length;
    var card = document.createElement('article');
    card.className = 'device-card';
    card.tabIndex = 0;
    card.setAttribute('role', 'button');
    card.setAttribute('aria-label', (text(p, 'name') || p.name) + ', ' + catLabel(p.category));
    card.innerHTML =
      '<span class="device-icon" style="--tint:' + esc(p.color || meta.color) + '">' + esc(p.icon || meta.icon) + '</span>' +
      '<span class="device-info"><strong>' + esc(text(p, 'name') || p.name) + '</strong>' +
      '<small>' + esc(catLabel(p.category)) + ' · ' + mapped + '/' + p.buttons.length + ' ' + esc(t('mappedCount')) + '</small></span>' +
      '<button class="star-btn" type="button" aria-label="' + esc(t('favorite')) + '">' + (p.favorite ? '★' : '☆') + '</button>';
    card.addEventListener('click', function (ev) {
      if (ev.target && ev.target.classList.contains('star-btn')) {
        ev.stopPropagation();
        p.favorite = !p.favorite;
        saveDb().then(renderHome).catch(function () {});
        return;
      }
      openRemote(p.id);
    });
    card.addEventListener('keydown', function (ev) {
      if (ev.key === 'Enter' || ev.key === ' ') { ev.preventDefault(); openRemote(p.id); }
    });
    return card;
  }

  function groupGrid(list) {
    var wrap = document.createElement('div');
    wrap.className = 'device-groups';
    var order = ['tv', 'ac', 'fan', 'stb', 'custom'];
    order.forEach(function (cat) {
      var group = list.filter(function (p) { return p.category === cat; });
      if (!group.length) return;
      var h = document.createElement('h3');
      h.textContent = catLabel(cat);
      wrap.appendChild(h);
      var grid = document.createElement('div');
      grid.className = 'device-grid';
      group.forEach(function (p) { grid.appendChild(deviceCard(p)); });
      wrap.appendChild(grid);
    });
    return wrap;
  }

  function renderHome() {
    if (!state.db) return;
    var q = state.search.trim().toLowerCase();
    var all = (state.db.profiles || []).slice();
    all.forEach(ensureLayout);
    var visible = all.filter(function (p) { return profileMatches(p, q); });

    var favs = visible.filter(function (p) { return p.favorite; });
    var recentIds = recents();
    var recent = [];
    recentIds.forEach(function (id) {
      for (var i = 0; i < visible.length; i++) if (visible[i].id === id) { recent.push(visible[i]); break; }
    });
    recent = recent.filter(function (p, i) { return recent.indexOf(p) === i && !p.favorite; });

    var quick = [];
    visible.forEach(function (p) {
      ensureLayout(p);
      p.buttons.forEach(function (b) {
        var s = signalFor(p, b);
        if (b.favorite && s) quick.push({ profile:p, button:b, signal:s });
      });
    });

    $('favoritesSection').classList.toggle('hidden', !favs.length);
    $('favoriteGrid').innerHTML = '';
    favs.forEach(function (p) { $('favoriteGrid').appendChild(deviceCard(p)); });

    $('recentSection').classList.toggle('hidden', !recent.length);
    $('recentGrid').innerHTML = '';
    recent.forEach(function (p) { $('recentGrid').appendChild(deviceCard(p)); });

    $('quickSection').classList.toggle('hidden', !quick.length);
    $('quickGrid').innerHTML = '';
    quick.forEach(function (qk) {
      var btn = document.createElement('button');
      btn.type = 'button';
      btn.className = 'quick-btn';
      btn.innerHTML = '<span>' + esc(buttonIcon(qk.button)) + '</span><span>' + esc(buttonLabel(qk.button)) + '</span>' +
                      '<small>' + esc(text(qk.profile, 'name')) + '</small>';
      btn.addEventListener('click', function () { sendMapped(qk.profile, qk.button, btn); });
      $('quickGrid').appendChild(btn);
    });

    var allGrid = $('allGrid');
    allGrid.innerHTML = '';
    if (visible.length) allGrid.appendChild(groupGrid(visible));
    $('emptyHome').classList.toggle('hidden', all.length > 0 || !!q);
    $('emptySearch').textContent = t('emptySearch');
    $('emptySearch').classList.toggle('hidden', !(q && !visible.length));

    $('homeSubtitle').textContent = all.length + ' ' + t('deviceCount');
  }

  // ---------------------------------------------------------------------
  // Remote view
  // ---------------------------------------------------------------------
  function currentProfile() {
    if (!state.db) return null;
    for (var i = 0; i < state.db.profiles.length; i++)
      if (state.db.profiles[i].id === state.currentProfileId) return state.db.profiles[i];
    return null;
  }

  function openRemote(id) {
    state.currentProfileId = id;
    state.editing = false;
    var p = currentProfile();
    if (!p) { showView('home'); return; }
    ensureLayout(p);
    rememberRecent(id);
    $('searchInput').value = '';
    state.search = '';
    renderRemote();
    showView('remote');
  }

  function remoteButtonEl(p, b) {
    var sig = signalFor(p, b);
    var mapped = !!sig;
    var unverified = !!(sig && sig.verified === false);
    var btn = document.createElement('button');
    btn.type = 'button';
    btn.className = 'remote-btn w' + (b.w || 1) + (mapped ? ' mapped' : ' unmapped') +
                    (b.color ? ' tinted' : '') + (b.key === 'ok' ? ' ok' : '');
    if (b.color) btn.style.setProperty('--tint', b.color);
    btn.setAttribute('aria-label', buttonLabel(b) + (mapped ? '' : ' — ' + t('notConfigured')));
    if (unverified) btn.title = t('unverifiedCode');
    btn.innerHTML = '<span class="rb-icon">' + esc(buttonIcon(b)) + '</span>' +
                    '<span class="rb-label">' + esc(buttonLabel(b)) + '</span>' +
                    (mapped ? '<span class="rb-dot' + (unverified ? ' warn' : '') + '" aria-hidden="true"></span>' : '<span class="rb-learn">' + esc(t('learn')) + '</span>') +
                    (state.editing ? '<span class="rb-edit" aria-hidden="true">✎</span>' : '');

    var pressTimer = null;
    var longPressed = false;
    function down() {
      longPressed = false;
      pressTimer = setTimeout(function () {
        longPressed = true;
        openLearn(p, b);
      }, 620);
    }
    function up() { if (pressTimer) { clearTimeout(pressTimer); pressTimer = null; } }
    btn.addEventListener('pointerdown', function (ev) {
      if (ev.button && ev.button !== 0) return;
      down();
    });
    btn.addEventListener('pointerup', up);
    btn.addEventListener('pointerleave', up);
    btn.addEventListener('pointercancel', up);
    btn.addEventListener('click', function (ev) {
      ev.preventDefault();
      if (longPressed) { longPressed = false; return; }
      if (state.editing) { openButtonEditor(p, b); return; }
      if (!signalFor(p, b)) { openLearn(p, b); return; }
      sendMapped(p, b, btn);
    });
    return btn;
  }

  function sectionEl(p, section, buttons) {
    var sec = document.createElement('section');
    sec.className = 'remote-section';
    var h = document.createElement('h3');
    h.textContent = t('sec' + section.charAt(0).toUpperCase() + section.slice(1));
    sec.appendChild(h);

    if (section === 'navigation') {
      var dpad = document.createElement('div');
      dpad.className = 'dpad';
      var byKey = {};
      buttons.forEach(function (b) { byKey[b.key] = b; });
      var order = [['', 'up', ''], ['left', 'ok', 'right'], ['', 'down', '']];
      order.forEach(function (row) {
        row.forEach(function (key) {
          if (!key) { dpad.appendChild(document.createElement('span')); return; }
          if (byKey[key]) dpad.appendChild(remoteButtonEl(p, byKey[key]));
          else dpad.appendChild(document.createElement('span'));
        });
      });
      sec.appendChild(dpad);
      var extra = buttons.filter(function (b) { return ['up','down','left','right','ok'].indexOf(b.key) < 0; });
      if (extra.length) {
        var eg = document.createElement('div');
        eg.className = 'remote-grid three';
        extra.forEach(function (b) { eg.appendChild(remoteButtonEl(p, b)); });
        sec.appendChild(eg);
      }
      return sec;
    }

    var grid = document.createElement('div');
    grid.className = 'remote-grid ' + (section === 'numbers' ? 'three' : (buttons.length === 1 ? 'one' : 'two'));
    buttons.forEach(function (b) { grid.appendChild(remoteButtonEl(p, b)); });
    sec.appendChild(grid);
    return sec;
  }

  function renderRemote() {
    var p = currentProfile();
    if (!p) { showView('home'); return; }
    ensureLayout(p);
    var meta = categoryMeta(p.category);
    $('remoteIcon').textContent = p.icon || meta.icon;
    $('remoteName').textContent = text(p, 'name') || p.name;
    $('remoteMeta').textContent = catLabel(p.category);
    $('remoteFav').textContent = p.favorite ? '★' : '☆';
    $('remoteFav').setAttribute('aria-pressed', p.favorite ? 'true' : 'false');
    $('editBar').classList.toggle('hidden', !state.editing);
    $('addButtonBtn').classList.toggle('hidden', p.category !== 'custom');
    $('remoteBody').innerHTML = '';

    var isAc = p.category === 'ac';
    $('acPanel').classList.toggle('hidden', !isAc);
    if (isAc) renderAcPanel(p);

    var sections = [];
    p.buttons.forEach(function (b) {
      var name = b.section || 'custom';
      var found = null;
      for (var i = 0; i < sections.length; i++) if (sections[i].name === name) found = sections[i];
      if (!found) { found = { name:name, buttons:[] }; sections.push(found); }
      found.buttons.push(b);
    });
    sections.forEach(function (s) { $('remoteBody').appendChild(sectionEl(p, s.name, s.buttons)); });

    if (!p.buttons.length) {
      var empty = document.createElement('p');
      empty.className = 'empty';
      empty.textContent = state.editing ? t('addButton') : t('emptyHome');
      $('remoteBody').appendChild(empty);
    }
    if (isAc) {
      var note = document.createElement('p');
      note.className = 'muted small center';
      note.textContent = t('longPressHint');
      $('remoteBody').appendChild(note);
    }
  }

  function acStateFor(p) {
    var all = {};
    try { all = JSON.parse(localStorage.getItem('rovixane-ac') || '{}'); } catch (e) { all = {}; }
    return all[p.id] || { temp:24, mode:'cool', fan:'auto' };
  }
  function setAcState(p, s) {
    var all = {};
    try { all = JSON.parse(localStorage.getItem('rovixane-ac') || '{}'); } catch (e) { all = {}; }
    all[p.id] = s;
    try { localStorage.setItem('rovixane-ac', JSON.stringify(all)); } catch (e) { /* ignore */ }
  }
  function renderAcPanel(p) {
    var s = acStateFor(p);
    $('acTemp').textContent = s.temp + '°';
    $('acMode').textContent = t('mode' + s.mode.charAt(0).toUpperCase() + s.mode.slice(1));
    $('acFan').textContent = t('fan' + s.fan.charAt(0).toUpperCase() + s.fan.slice(1));
  }

  function sendMapped(p, b, el) {
    var s = signalFor(p, b);
    if (!s) { openLearn(p, b); return; }
    if (el) el.classList.add('busy');
    if (p.category === 'ac') {
      var ac = acStateFor(p);
      if (b.key === 'tempup') ac.temp = Math.min(30, ac.temp + 1);
      if (b.key === 'tempdown') ac.temp = Math.max(16, ac.temp - 1);
      if (b.key === 'mode') {
        var modes = ['cool','heat','dry','fan','auto'];
        ac.mode = modes[(modes.indexOf(ac.mode) + 1) % modes.length];
      }
      if (b.key === 'fan') {
        var fans = ['auto','low','mid','high'];
        ac.fan = fans[(fans.indexOf(ac.fan) + 1) % fans.length];
      }
      setAcState(p, ac);
      renderAcPanel(p);
    }
    post('/api/send', { profileId:p.id, signalId:s.id })
      .then(function () {
        if (el) { el.classList.remove('busy'); el.classList.add('sent'); setTimeout(function () { el.classList.remove('sent'); }, 450); }
        toast(t('sent'));
      })
      .catch(function (e) {
        if (el) el.classList.remove('busy');
        toast(e.message || t('sendFailed'), true);
      });
  }

  // ---------------------------------------------------------------------
  // Sheets, learning and editors
  // ---------------------------------------------------------------------
  function openSheet(html, onSave, opts) {
    opts = opts || {};
    state.sheetOnSave = onSave || null;
    state.sheetReturnFocus = document.activeElement;
    $('sheetBody').innerHTML = html;
    $('sheetSave').classList.toggle('hidden', !onSave);
    $('sheetSave').textContent = opts.saveLabel || t('save');
    $('sheetSave').className = 'btn ' + (opts.danger ? 'danger' : 'primary');
    $('sheetCancel').textContent = t('cancel');
    $('sheet').classList.remove('hidden');
    var first = $('sheetBody').querySelector('input, select, textarea, button');
    if (first) { try { first.focus(); } catch (e) { /* ignore */ } }
  }
  function closeSheet() {
    $('sheet').classList.add('hidden');
    state.sheetOnSave = null;
    stopLearningPolling();
    if (state.sheetReturnFocus && state.sheetReturnFocus.focus) {
      try { state.sheetReturnFocus.focus(); } catch (e) { /* ignore */ }
    }
  }
  function sheetTitle(title) { return '<h2 id="sheetTitle">' + esc(title) + '</h2>'; }
  function inputField(id, label, value, type, attrs) {
    return '<label><span>' + esc(label) + '</span><input id="' + id + '" type="' + (type || 'text') +
           '" value="' + esc(value || '') + '" ' + (attrs || '') + '></label>';
  }
  function selectField(id, label, options, selected) {
    var out = '<label><span>' + esc(label) + '</span><select id="' + id + '">';
    options.forEach(function (o) {
      out += '<option value="' + esc(o.value) + '"' + (String(o.value) === String(selected) ? ' selected' : '') + '>' + esc(o.label) + '</option>';
    });
    return out + '</select></label>';
  }
  function iconOptions(selected) {
    return '<div class="icon-picker" id="iconPicker">' + ICONS.map(function (ic) {
      return '<button type="button" class="icon-choice' + (ic === selected ? ' sel' : '') + '" data-icon="' + esc(ic) + '">' + esc(ic) + '</button>';
    }).join('') + '</div><input type="hidden" id="mIcon" value="' + esc(selected || '⭐') + '">';
  }
  function bindIconPicker() {
    var picker = $('iconPicker');
    if (!picker) return;
    picker.addEventListener('click', function (ev) {
      var b = ev.target.closest('.icon-choice');
      if (!b) return;
      picker.querySelectorAll('.icon-choice').forEach(function (x) { x.classList.remove('sel'); });
      b.classList.add('sel');
      $('mIcon').value = b.getAttribute('data-icon');
    });
  }

  function stopLearningPolling() {
    if (state.poll) { clearInterval(state.poll); state.poll = null; }
    if (state.learnTimer) { clearTimeout(state.learnTimer); state.learnTimer = null; }
    if (state.learningActive) {
      state.learningActive = false;
      post('/api/learn/cancel', {}).catch(function () {});
    }
  }

  function openLearn(p, b) {
    var existing = signalFor(p, b);
    if (existing && !confirm(t('confirmOverwrite'))) return;
    toast(t('learning'));
    state.learningActive = true;
    post('/api/learn/start', {}).then(function () {
      openSheet(sheetTitle(t('learnForButton') + ': ' + buttonLabel(b)) +
        '<div class="learn-box" id="learnBox"><span class="spinner" aria-hidden="true"></span><p>' +
        esc(t('learnWaiting')) + '</p></div>', null, { hideSave:true });
      $('sheetCancel').textContent = t('cancel');
      state.poll = setInterval(function () { pollLearnSheet(p, b, existing); }, 420);
      state.learnTimer = setTimeout(function () {
        stopLearningPolling();
        showLearnError(t('learnInvalid'), function () { openLearn(p, b); });
      }, 30000);
    }).catch(function (e) {
      state.learningActive = false;
      toast(e.message || t('learnInvalid'), true);
    });
  }

  function showLearnError(message, retry) {
    var box = $('learnBox');
    if (!box) return;
    box.innerHTML = '<p class="err-text">' + esc(message) + '</p>' +
      '<button type="button" class="btn ghost" id="learnRetryBtn">' + esc(t('learnRetry')) + '</button>';
    var r = $('learnRetryBtn');
    if (r) r.addEventListener('click', function () { stopLearningPolling(); retry(); });
  }

  function pollLearnSheet(p, b, existing) {
    api('/api/learn/status').then(function (r) {
      if (!r || r.active) return;
      stopLearningPolling();
      var cap = r.capture;
      if (!cap || cap.kind === 'error') {
        var msg = cap && cap.error ? cap.error : (cap && cap.overflow ? t('learnOverflow') : t('learnInvalid'));
        showLearnError(msg, function () { openLearn(p, b); });
        return;
      }
      var lines = [t('protocol') + ': ' + (cap.protocol || '—'),
                   t('hex') + ': ' + (cap.hex || '—'),
                   t('bits') + ': ' + (cap.bits || 0),
                   t('khz') + ': ' + (cap.khz || 38)];
      if (cap.timings && cap.timings.length) lines.push(t('raw') + ': ' + cap.timings.length);
      if (cap.unsupported) lines.push('⚠ ' + t('learnUnsupported'));
      var box = $('learnBox');
      box.innerHTML = '<p class="ok-text">' + esc(t('learnReady')) + '</p><pre class="result">' +
                      esc(lines.join('\n')) + '</pre>' +
                      '<button type="button" class="btn primary" id="learnSaveBtn">' + esc(t('saveSignal')) + '</button>' +
                      '<button type="button" class="btn ghost" id="learnRetryBtn">' + esc(t('learnRetry')) + '</button>';
      $('learnSaveBtn').addEventListener('click', function () {
        assignCapture(p, b, cap, existing);
      });
      $('learnRetryBtn').addEventListener('click', function () {
        closeSheet();
        setTimeout(function () { openLearn(p, b); }, 120);
      });
    }).catch(function () {
      stopLearningPolling();
      showLearnError(t('learnInvalid'), function () { openLearn(p, b); });
    });
  }

  function assignCapture(p, b, cap, existing) {
    if (!cap || cap.kind === 'error') return;
    var sig = existing || { id: uid('s') };
    sig.label = buttonLabel(b);
    sig.labelBn = b.labelBn || (state.lang === 'bn' ? sig.label : '');
    sig.icon = buttonIcon(b);
    sig.color = b.color || '#4f8cff';
    sig.repeat = 1;
    sig.verified = true;
    if (cap.kind === 'raw') {
      sig.kind = 'raw';
      sig.protocol = 'RAW';
      sig.timings = cap.timings || [];
      sig.khz = cap.khz || 38;
      delete sig.value;
      delete sig.bits;
    } else {
      sig.kind = 'protocol';
      sig.protocol = cap.protocol;
      sig.value = cap.hex;
      sig.bits = cap.bits;
      delete sig.timings;
      delete sig.khz;
    }
    if (!existing) {
      p.signals = p.signals || [];
      p.signals.push(sig);
    }
    b.signalId = sig.id;
    saveDb().then(function () {
      toast(t('signalSaved'));
      closeSheet();
      renderRemote();
      renderHome();
    }).catch(function () {});
  }

  // ---------------------------------------------------------------------
  // Remote menu and profile editors
  // ---------------------------------------------------------------------
  function openRemoteMenu(p) {
    var html = sheetTitle(t('profileMenu')) +
      '<div class="menu-list">' +
      '<button type="button" class="menu-item" data-act="edit">✎ ' + esc(t('editLayout')) + '</button>' +
      '<button type="button" class="menu-item" data-act="rename">✎ ' + esc(t('rename')) + '</button>' +
      '<button type="button" class="menu-item" data-act="duplicate">⧉ ' + esc(t('duplicate')) + '</button>' +
      '<button type="button" class="menu-item" data-act="export">⇩ ' + esc(t('exportProfile')) + '</button>' +
      '<button type="button" class="menu-item danger" data-act="delete">🗑 ' + esc(t('delete')) + '</button>' +
      '</div>';
    openSheet(html, null);
    var box = $('sheetBody');
    box.addEventListener('click', function (ev) {
      var item = ev.target.closest('.menu-item');
      if (!item) return;
      var act = item.getAttribute('data-act');
      closeSheet();
      setTimeout(function () {
        if (act === 'edit') { state.editing = !state.editing; renderRemote(); }
        else if (act === 'rename') openProfileEditor(p);
        else if (act === 'duplicate') duplicateProfile(p);
        else if (act === 'export') exportProfile(p);
        else if (act === 'delete') confirmDeleteProfile(p);
      }, 80);
    });
  }

  function openProfileEditor(p) {
    var isNew = !p;
    var draft = p || { name:'', nameBn:'', icon:'📺', color:'#4f8cff', category:'tv' };
    var cats = CATEGORIES.map(function (c) { return { value:c.id, label:t(c.label) }; });
    var html = sheetTitle(isNew ? t('addRemoteTitle') : t('rename')) +
      (isNew ? '<p class="muted small">' + esc(t('addRemoteHelp')) + '</p>' : '') +
      inputField('mName', t('profileName'), draft.name, 'text', 'maxlength="40"') +
      inputField('mNameBn', t('profileNameBn'), draft.nameBn, 'text', 'maxlength="40"') +
      (isNew ? selectField('mCategory', t('profileCategory'), cats, draft.category) : '') +
      '<label><span>' + esc(t('profileColor')) + '</span><input id="mColor" type="color" value="' + esc(draft.color || '#4f8cff') + '"></label>' +
      iconOptions(draft.icon || '📺');
    openSheet(html, function () {
      var name = $('mName').value.trim();
      if (!name) { toast(t('errNameRequired'), true); $('mName').focus(); return; }
      var profile = p;
      if (isNew) {
        var meta = categoryMeta($('mCategory').value);
        profile = {
          id: uid('p'), name: name, nameBn: $('mNameBn').value.trim(),
          icon: $('mIcon').value || meta.icon, color: $('mColor').value || meta.color,
          category: $('mCategory').value, favorite: false, signals: [], buttons: []
        };
        ensureLayout(profile);
        state.db.profiles.push(profile);
      } else {
        profile.name = name;
        profile.nameBn = $('mNameBn').value.trim();
        profile.icon = $('mIcon').value;
        profile.color = $('mColor').value;
      }
      saveDb().then(function () {
        closeSheet();
        openRemote(profile.id);
        renderHome();
      }).catch(function () {});
    }, { saveLabel: isNew ? t('createRemote') : t('save') });
    bindIconPicker();
  }

  function duplicateProfile(p) {
    var c = clone(p);
    c.id = uid('p');
    c.name = (c.name || '') + ' (copy)';
    c.nameBn = (c.nameBn || '') + ' (কপি)';
    c.favorite = false;
    (c.signals || []).forEach(function (s) { s.id = uid('s'); });
    var map = {};
    (p.signals || []).forEach(function (s, i) { map[s.id] = c.signals[i].id; });
    (c.buttons || []).forEach(function (b) {
      b.id = uid('b');
      if (b.signalId) b.signalId = map[b.signalId] || '';
    });
    state.db.profiles.push(c);
    saveDb().then(function () { openRemote(c.id); renderHome(); }).catch(function () {});
  }

  function confirmDeleteProfile(p) {
    openSheet(sheetTitle(t('delete') + ' — ' + (text(p, 'name') || p.name)) +
      '<p class="muted">' + esc(t('confirmDeleteProfile')) + '</p>', function () {
      state.db.profiles = state.db.profiles.filter(function (x) { return x.id !== p.id; });
      if (state.currentProfileId === p.id) state.currentProfileId = null;
      saveDb().then(function () { closeSheet(); showView('home'); renderHome(); }).catch(function () {});
    }, { danger:true, saveLabel:t('delete') });
  }

  function exportProfile(p) {
    try {
      var payload = { format:'ihr-profile', version:2, profile:p };
      var blob = new Blob([JSON.stringify(payload, null, 2)], { type:'application/json' });
      var a = document.createElement('a');
      a.href = URL.createObjectURL(blob);
      a.download = 'rovixane-remote-' + (p.name || 'remote').replace(/[^A-Za-z0-9_-]+/g, '-') + '.json';
      a.click();
      setTimeout(function () { URL.revokeObjectURL(a.href); }, 1000);
    } catch (e) { toast(t('saveFailed'), true); }
  }

  function openButtonEditor(p, b) {
    var isNew = !b;
    var draft = b || { id:uid('b'), key:'', label:'', labelBn:'', icon:'⭐', color:'', w:1, section:'custom', favorite:false, signalId:'' };
    var options = [{ value:'', label:t('notMapped') }];
    (p.signals || []).forEach(function (s) {
      options.push({ value:s.id, label:s.label + ' · ' + (s.protocol || s.kind) });
    });
    options.push({ value:'__learn__', label:'＋ ' + t('learnNew') });
    var html = sheetTitle(isNew ? t('addButton') : t('editButton')) +
      inputField('mLabel', t('name'), draft.label, 'text', 'maxlength="40"') +
      inputField('mLabelBn', t('nameBn'), draft.labelBn, 'text', 'maxlength="40"') +
      selectField('mSignal', t('assignSignal'), options, draft.signalId || '') +
      selectField('mWidth', t('width'), [{value:'1',label:'1'},{value:'2',label:'2'},{value:'3',label:'3'}], draft.w || 1) +
      '<label class="check"><input id="mFav" type="checkbox"' + (draft.favorite ? ' checked' : '') + '><span>' + t('favorite') + '</span></label>' +
      '<label><span>' + esc(t('color')) + '</span><input id="mColor" type="color" value="' + esc(draft.color || '#4f8cff') + '"></label>' +
      iconOptions(draft.icon || '⭐') +
      (!isNew ? '<div class="row wrap"><button type="button" class="btn ghost" id="mUp">↑ ' + esc(t('moveUp')) + '</button>' +
        '<button type="button" class="btn ghost" id="mDown">↓ ' + esc(t('moveDown')) + '</button>' +
        '<button type="button" class="btn danger" id="mDelete">' + esc(t('deleteButton')) + '</button></div>' : '');
    openSheet(html, function () {
      var label = $('mLabel').value.trim();
      if (!label) { toast(t('errNameRequired'), true); $('mLabel').focus(); return; }
      var target = b;
      if (isNew) {
        target = draft;
        p.buttons = p.buttons || [];
        p.buttons.push(target);
      }
      target.label = label;
      target.labelBn = $('mLabelBn').value.trim();
      target.icon = $('mIcon').value;
      target.color = $('mColor').value;
      target.w = parseInt($('mWidth').value, 10) || 1;
      target.favorite = $('mFav').checked;
      var chosen = $('mSignal').value;
      var wantLearn = chosen === '__learn__';
      if (!wantLearn) target.signalId = chosen || '';
      saveDb().then(function () {
        closeSheet();
        renderRemote();
        renderHome();
        if (wantLearn) setTimeout(function () { openLearn(p, target); }, 120);
      }).catch(function () {});
    }, { saveLabel:t('save') });
    bindIconPicker();
    if (!isNew) {
      var move = function (dir) {
        var idx = p.buttons.indexOf(b);
        var next = idx + dir;
        if (idx < 0 || next < 0 || next >= p.buttons.length) return;
        p.buttons[idx] = p.buttons[next];
        p.buttons[next] = b;
        saveDb().then(function () { closeSheet(); renderRemote(); }).catch(function () {});
      };
      $('mUp').addEventListener('click', function () { move(-1); });
      $('mDown').addEventListener('click', function () { move(1); });
      $('mDelete').addEventListener('click', function () {
        if (!confirm(t('confirmDeleteSignal'))) return;
        p.buttons = p.buttons.filter(function (x) { return x.id !== b.id; });
        saveDb().then(function () { closeSheet(); renderRemote(); }).catch(function () {});
      });
    }
  }

  // ---------------------------------------------------------------------
  // Settings
  // ---------------------------------------------------------------------
  function applyTheme(theme) {
    state.theme = theme === 'light' ? 'light' : 'dark';
    document.documentElement.setAttribute('data-theme', state.theme);
    $('themeBtn').textContent = state.theme === 'light' ? '☀' : '☾';
    try { localStorage.setItem('rovixane-theme', state.theme); } catch (e) { /* ignore */ }
  }

  function fillConfig() {
    var c = state.cfg; if (!c) return;
    $('cfgDeviceName').value = c.deviceName || '';
    $('cfgApSsid').value = c.apSsid || '';
    $('cfgWifiSsid').value = c.wifiSsid || '';
    $('cfgApPass').value = '';
    $('cfgWifiPass').value = '';
    $('cfgHttpPass').value = '';
    $('cfgApPass').placeholder = c.apPasswordSet ? t('secretPlaceholder') : '';
    $('cfgWifiPass').placeholder = c.wifiPasswordSet ? t('secretPlaceholder') : '';
    $('cfgHttpPass').placeholder = c.httpPasswordSet ? t('secretPlaceholder') : '';
    $('cfgApOpen').checked = !c.apPasswordSet;
    $('cfgCaptive').checked = !!c.captivePortal;
    $('cfgAuth').checked = !!c.httpAuth;
    $('cfgUser').value = c.httpUser || 'admin';
    $('cfgRepeat').value = c.irRepeat || 1;
    $('cfgLocale').value = c.locale === 'bn' ? 'bn' : 'en';
    $('cfgTheme').value = state.theme;
    $('fwVersion').textContent = c.version || '—';
    $('otaSupport').textContent = c.otaSupported ? t('yes') : t('no');
    $('deviceName').textContent = c.deviceName || 'ROVIXANE IR';
    $('aboutLine').textContent = (c.product || 'ROVIXANE IR') + ' · v' + (c.version || '') + ' · ' + (c.build || '');
  }

  function showSetup(show) {
    $('setupCard').classList.toggle('hidden', !show);
    $('setupPasswordCard').classList.toggle('hidden', !show);
  }

  function collectConfig() {
    var payload = {
      deviceName: $('cfgDeviceName').value.trim(),
      apSsid: $('cfgApSsid').value.trim(),
      wifiSsid: $('cfgWifiSsid').value.trim(),
      captivePortal: $('cfgCaptive').checked,
      httpAuth: $('cfgAuth').checked,
      httpUser: $('cfgUser').value.trim(),
      irRepeat: Math.max(1, Math.min(10, parseInt($('cfgRepeat').value, 10) || 1)),
      locale: $('cfgLocale').value
    };
    var apPass = $('cfgApPass').value;
    if ($('cfgApOpen').checked) payload.apPassword = '';
    else if (apPass) payload.apPassword = apPass;
    var wifiPass = $('cfgWifiPass').value;
    if (wifiPass) payload.wifiPassword = wifiPass;
    var httpPass = $('cfgHttpPass').value;
    if (httpPass) payload.httpPassword = httpPass;
    return payload;
  }

  function submitConfig() {
    var c = collectConfig();
    if (!c.deviceName) { toast(t('errDeviceName'), true); $('cfgDeviceName').focus(); return; }
    if (!c.apSsid) { toast(t('errApSsid'), true); $('cfgApSsid').focus(); return; }
    if (c.apPassword && (c.apPassword.length < 8 || c.apPassword.length > 63)) {
      toast(t('errApPassword'), true); $('cfgApPass').focus(); return;
    }
    if (c.wifiPassword && c.wifiPassword.length > 64) {
      toast(t('errWifiPassword'), true); $('cfgWifiPass').focus(); return;
    }
    if (c.httpAuth) {
      if (!c.httpUser) { toast(t('errHttpUser'), true); $('cfgUser').focus(); return; }
      var hasExisting = state.cfg && state.cfg.httpPasswordSet;
      if (!hasExisting && !c.httpPassword) {
        toast(t('errHttpPassword'), true); $('cfgHttpPass').focus(); return;
      }
      if (c.httpPassword && c.httpPassword.length < 8) {
        toast(t('errHttpPassword'), true); $('cfgHttpPass').focus(); return;
      }
    }
    post('/api/config', c).then(function (r) {
      state.cfg.deviceName = c.deviceName;
      state.cfg.apSsid = c.apSsid;
      state.cfg.wifiSsid = c.wifiSsid;
      state.cfg.captivePortal = c.captivePortal;
      state.cfg.httpAuth = c.httpAuth;
      state.cfg.httpUser = c.httpUser;
      state.cfg.irRepeat = c.irRepeat;
      state.cfg.locale = c.locale;
      if (c.httpPassword) state.cfg.httpPasswordSet = true;
      if (c.apPassword) state.cfg.apPasswordSet = true;
      if (c.apPassword === '') state.cfg.apPasswordSet = false;
      if (c.wifiPassword) state.cfg.wifiPasswordSet = true;
      state.lang = c.locale === 'bn' ? 'bn' : 'en';
      applyI18n();
      fillConfig();
      renderAll();
      toast(t('savedSettings'));
      $('restartNotice').classList.toggle('hidden', !(r && r.restartRequired));
    }).catch(function (e) { toast(e.message || t('configSaveFailed'), true); });
  }

  function submitSetup(ev) {
    ev.preventDefault();
    var user = $('setupUser').value.trim() || 'admin';
    var pass = $('setupPassword').value;
    var pass2 = $('setupPassword2').value;
    if (pass.length < 8 || pass.length > 64) { toast(t('errSetupPassword'), true); return; }
    if (pass !== pass2) { toast(t('errPasswordMismatch'), true); return; }
    post('/api/setup', { user:user, password:pass }).then(function () {
      rememberAuth(user, pass);
      toast(t('setupComplete'));
      state.cfg.setupRequired = false;
      state.cfg.httpAuth = true;
      state.cfg.httpUser = user;
      showSetup(false);
      loadAll();
    }).catch(function (e) { toast(e.message || t('setupRequiredError'), true); });
  }

  function renderDiagnostics() {
    var s = state.status || {};
    var wifiState = s.apMode ? t('apMode') : (s.wifiConnected ? (s.wifiSsid || t('staMode')) : t('connecting'));
    var rows = [
      [t('diagFirmware'), s.version], [t('diagBuild'), s.build],
      [t('diagUptime'), s.uptimeSec], [t('diagHeap'), s.freeHeap],
      [t('diagFs'), s.fsUsed + ' / ' + s.fsTotal],
      [t('diagSketch'), s.sketchSize + ' / ' + s.freeSketchSpace],
      [t('diagSignals'), s.profileCount + ' / ' + s.signalCount],
      [t('diagIp'), s.ip], [t('diagWifi'), wifiState],
      [t('diagRssi'), s.rssi], [t('diagReset'), resetLabel(s.resetReason)],
      [t('diagLocale'), s.locale], [t('diagSetup'), s.setupRequired ? t('yes') : t('no')]
    ];
    $('diagOut').textContent = rows.map(function (r) { return r[0] + ': ' + (r[1] == null ? '—' : r[1]); }).join('\n');
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

  // ---------------------------------------------------------------------
  // Views
  // ---------------------------------------------------------------------
  function showView(name) {
    state.view = name;
    ['home','remote','learn','settings'].forEach(function (v) {
      $('view-' + v).classList.toggle('active', v === name);
    });
    var navMap = { home:'home', remote:'home', learn:'learn', settings:'settings' };
    document.querySelectorAll('.nav-btn').forEach(function (b) {
      b.classList.toggle('active', b.getAttribute('data-nav') === navMap[name]);
    });
    if (name === 'home') renderHome();
    if (name === 'remote') renderRemote();
    if (name === 'settings') {
      fillConfig();
      renderDiagnostics();
      api('/api/status').then(function (s) { state.status = s; renderDiagnostics(); }).catch(function () {});
    }
    window.scrollTo(0, 0);
  }

  function renderAll() {
    renderHome();
    if (state.currentProfileId) renderRemote();
    if (state.view === 'settings') { fillConfig(); renderDiagnostics(); }
  }

  function renderStatusPill() {
    var s = state.status || {};
    var pill = $('connPill');
    if (s.apMode) { pill.textContent = t('apMode'); pill.className = 'pill ap'; }
    else if (s.wifiConnected) { pill.textContent = s.wifiSsid || t('online'); pill.className = 'pill on'; }
    else { pill.textContent = t('connecting'); pill.className = 'pill'; }
    pill.title = s.ip || '';
  }

  // ---------------------------------------------------------------------
  // Firmware update
  // ---------------------------------------------------------------------
  function validateOtaFile(file) {
    if (!file) { toast(t('noFile'), true); return false; }
    if (!/\.bin$/i.test(file.name)) { toast(t('errFileType'), true); return false; }
    if (file.size < 10 * 1024 || file.size > 1000 * 1024) { toast(t('errFileSize'), true); return false; }
    return true;
  }

  function chooseOtaFile() {
    var f = $('otaFile').files[0];
    state.otaFile = f || null;
    $('otaInfo').textContent = f ? (f.name + ' · ' + Math.round(f.size / 1024) + ' KB') : '';
    $('otaStart').disabled = !f || state.uploading || !(state.cfg && state.cfg.otaSupported);
    $('otaStatus').textContent = f ? t('otaReady') : '';
  }

  function startOta() {
    var f = state.otaFile || ($('otaFile').files && $('otaFile').files[0]);
    if (!validateOtaFile(f)) return;
    if (!state.cfg || !state.cfg.otaSupported) { toast(t('errOtaUnsupported'), true); return; }
    openSheet(sheetTitle(t('otaTitle')) +
      '<p class="muted">' + esc(t('otaConfirm')) + '</p>' +
      '<p class="mono small">' + esc(f.name) + ' · ' + Math.round(f.size / 1024) + ' KB</p>',
      function () { closeSheet(); uploadFirmware(f); }, { danger:true, saveLabel:t('uploadFirmware') });
  }

  function uploadFirmware(file) {
    state.uploading = true;
    $('otaStart').disabled = true;
    $('otaProgressWrap').classList.remove('hidden');
    $('otaProgress').style.width = '0%';
    $('otaStatus').textContent = t('otaUploading');
    var fd = new FormData();
    fd.append('firmware', file, file.name);
    var xhr = new XMLHttpRequest();
    xhr.open('POST', '/api/ota');
    xhr.setRequestHeader('X-ROVIXANE-CSRF', '1');
    var token = authToken();
    if (token) xhr.setRequestHeader('Authorization', 'Basic ' + token);
    xhr.upload.onprogress = function (ev) {
      if (!ev.lengthComputable) return;
      var pct = Math.max(1, Math.round(ev.loaded * 100 / ev.total));
      $('otaProgress').style.width = pct + '%';
    };
    xhr.onload = function () {
      state.uploading = false;
      $('otaStart').disabled = false;
      var data = null;
      try { data = JSON.parse(xhr.responseText); } catch (e) { data = null; }
      if (xhr.status >= 200 && xhr.status < 300 && data && data.ok) {
        $('otaProgress').style.width = '100%';
        $('otaStatus').textContent = t('otaSuccess');
        toast(t('otaSuccess'));
        setTimeout(function () {
          api('/api/status').then(function (s) {
            state.status = s;
            $('otaStatus').textContent = t('otaSuccess') + ' v' + s.version;
          }).catch(function () { location.reload(); });
        }, 7000);
      } else {
        var msg = (data && data.error) ? data.error : t('otaFailed');
        $('otaStatus').textContent = msg + ' ' + t('otaRecovery');
        toast(msg, true);
      }
    };
    xhr.onerror = function () {
      state.uploading = false;
      $('otaStart').disabled = false;
      $('otaStatus').textContent = t('otaFailed') + ' ' + t('otaRecovery');
      toast(t('otaFailed'), true);
    };
    xhr.send(fd);
  }

  // ---------------------------------------------------------------------
  // Backup / restore / import
  // ---------------------------------------------------------------------
  function exportBackup() {
    try {
      var payload = {
        format:'ihr-backup', version:2,
        exportedAt:new Date().toISOString(),
        config:state.cfg,
        signals:state.db
      };
      var blob = new Blob([JSON.stringify(payload, null, 2)], { type:'application/json' });
      var a = document.createElement('a');
      a.href = URL.createObjectURL(blob);
      a.download = 'rovixane-ir-backup-' + new Date().toISOString().slice(0, 10) + '.json';
      a.click();
      setTimeout(function () { URL.revokeObjectURL(a.href); }, 1000);
      toast(t('exported'));
    } catch (e) { toast(t('saveFailed'), true); }
  }

  function applyBackup(data) {
    if (!data || data.format !== 'ihr-backup' || !data.config || !data.signals) {
      toast(t('backupInvalid'), true); return;
    }
    var cfg = data.config;
    var payload = {
      deviceName: cfg.deviceName, apSsid: cfg.apSsid, wifiSsid: cfg.wifiSsid,
      captivePortal: cfg.captivePortal, httpAuth: cfg.httpAuth, httpUser: cfg.httpUser,
      irRepeat: cfg.irRepeat, locale: cfg.locale,
      apPassword: cfg.apPassword, wifiPassword: cfg.wifiPassword, httpPassword: cfg.httpPassword
    };
    post('/api/config', payload).then(function (r) {
      return put('/api/signals', data.signals).then(function () {
        toast(t('restored'));
        if (r && r.restartRequired) $('restartNotice').classList.remove('hidden');
        loadAll();
      });
    }).catch(function (e) { toast(e.message || t('saveFailed'), true); });
  }

  function importProfile(data) {
    if (!data || data.format !== 'ihr-profile' || !data.profile) {
      toast(t('profileImportInvalid'), true); return;
    }
    var p = clone(data.profile);
    p.id = uid('p');
    p.favorite = false;
    var map = {};
    (p.signals || []).forEach(function (s) { var old = s.id; s.id = uid('s'); map[old] = s.id; });
    (p.buttons || []).forEach(function (b) {
      b.id = uid('b');
      if (b.signalId) b.signalId = map[b.signalId] || '';
    });
    state.db.profiles.push(p);
    saveDb().then(function () { toast(t('profileImported')); renderHome(); }).catch(function () {});
  }

  function confirmRestart() {
    openSheet(sheetTitle(t('restart')) + '<p class="muted">' + esc(t('confirmRestart')) + '</p>', function () {
      closeSheet();
      toast(t('restarting'));
      post('/api/restart', {}).catch(function () {}).then(function () {
        setTimeout(function () { location.reload(); }, 1600);
      });
    }, { saveLabel:t('restart') });
  }

  function confirmFactory() {
    openSheet(sheetTitle(t('factoryReset')) +
      '<p class="muted">' + esc(t('confirmFactory')) + '</p>' +
      '<label><span>' + esc(t('confirmFactory')) + '</span><input id="factoryConfirm" autocomplete="off"></label>',
      function () {
        if ($('factoryConfirm').value.trim() !== 'RESET') { toast(t('confirmFactory'), true); return; }
        post('/api/factory-reset', { confirm:'RESET' }).then(function () {
          closeSheet();
          toast(t('factoryHint'));
          setTimeout(function () { location.assign('http://192.168.4.1/'); }, 3000);
        }).catch(function (e) { toast(e.message || t('requestFailed'), true); });
      }, { danger:true, saveLabel:t('factoryReset') });
  }

  // ---------------------------------------------------------------------
  // i18n and bindings
  // ---------------------------------------------------------------------
  function applyI18n() {
    document.documentElement.lang = state.lang;
    document.documentElement.setAttribute('data-lang', state.lang);
    document.title = t('appTitle');
    var meta = document.querySelector('meta[name="description"]');
    if (meta) meta.setAttribute('content', t('metaDescription'));
    document.querySelectorAll('[data-i18n]').forEach(function (el) {
      var key = el.getAttribute('data-i18n');
      if (el.childElementCount === 0) el.textContent = t(key);
      else if (el.firstChild && el.firstChild.nodeType === 3) el.firstChild.nodeValue = t(key);
    });
    var attrs = [['data-i18n-aria', 'aria-label'], ['data-i18n-title', 'title'], ['data-i18n-placeholder', 'placeholder']];
    attrs.forEach(function (pair) {
      document.querySelectorAll('[' + pair[0] + ']').forEach(function (el) {
        el.setAttribute(pair[1], t(el.getAttribute(pair[0])));
      });
    });
    $('langBtn').textContent = state.lang === 'bn' ? 'EN' : 'বাং';
  }

  function advancedLearnStart() {
    state.advLearned = null;
    $('learnResult').classList.add('hidden');
    $('learnSave').classList.add('hidden');
    $('learnProgress').classList.remove('hidden');
    stopLearningPolling();
    state.learningActive = true;
    post('/api/learn/start', {}).then(function () {
      state.poll = setInterval(function () {
        api('/api/learn/status').then(function (r) {
          if (!r || r.active) return;
          stopLearningPolling();
          $('learnProgress').classList.add('hidden');
          var cap = r.capture;
          if (!cap || cap.kind === 'error') {
            $('learnResult').textContent = cap && cap.error ? cap.error : t('learnInvalid');
            $('learnResult').classList.remove('hidden');
            return;
          }
          state.advLearned = cap;
          var lines = [t('protocol') + ': ' + (cap.protocol || '—'),
                       t('hex') + ': ' + (cap.hex || '—'),
                       t('bits') + ': ' + (cap.bits || 0),
                       t('khz') + ': ' + (cap.khz || 38)];
          if (cap.timings && cap.timings.length) lines.push(t('raw') + ': ' + cap.timings.join(', '));
          if (cap.unsupported) lines.push('⚠ ' + t('learnUnsupported'));
          $('learnResult').textContent = lines.join('\n');
          $('learnResult').classList.remove('hidden');
          $('learnSave').classList.remove('hidden');
        }).catch(function () {
          stopLearningPolling();
          $('learnProgress').classList.add('hidden');
          toast(t('learnInvalid'), true);
        });
      }, 450);
      state.learnTimer = setTimeout(function () {
        stopLearningPolling();
        $('learnProgress').classList.add('hidden');
        $('learnResult').textContent = t('learnInvalid');
        $('learnResult').classList.remove('hidden');
      }, 30000);
    }).catch(function (e) { state.learningActive = false; toast(e.message || t('learnInvalid'), true); });
  }

  function advancedLearnSave() {
    var cap = state.advLearned;
    if (!cap) return;
    var p = currentProfile() || (state.db.profiles[0] || null);
    if (!p) { toast(t('noProfiles'), true); return; }
    ensureLayout(p);
    var sig = {
      id: uid('s'), label: cap.protocol || t('button'), labelBn: '',
      icon: '⭐', color: '#4f8cff', repeat: 1, verified: true
    };
    if (cap.kind === 'raw') {
      sig.kind = 'raw'; sig.protocol = 'RAW'; sig.timings = cap.timings || []; sig.khz = cap.khz || 38;
    } else {
      sig.kind = 'protocol'; sig.protocol = cap.protocol; sig.value = cap.hex; sig.bits = cap.bits;
    }
    p.signals = p.signals || [];
    p.signals.push(sig);
    p.buttons = p.buttons || [];
    p.buttons.push({
      id: uid('b'), key: '', section: 'custom', label: sig.label, labelBn: '',
      icon: '⭐', color: '#4f8cff', w: 1, favorite: false, signalId: sig.id
    });
    saveDb().then(function () {
      toast(t('signalSaved'));
      $('learnSave').classList.add('hidden');
      renderAll();
    }).catch(function () {});
  }

  function bind() {
    $('langBtn').addEventListener('click', function () {
      state.lang = state.lang === 'en' ? 'bn' : 'en';
      applyI18n();
      renderAll();
      post('/api/config', { locale:state.lang }).catch(function () {});
    });
    $('themeBtn').addEventListener('click', function () {
      applyTheme(state.theme === 'dark' ? 'light' : 'dark');
      if (state.view === 'settings') fillConfig();
    });
    $('cfgTheme').addEventListener('change', function () {
      applyTheme(this.value);
      if (state.view === 'settings') fillConfig();
    });
    document.querySelectorAll('.nav-btn').forEach(function (b) {
      b.addEventListener('click', function () { showView(b.getAttribute('data-nav')); });
    });
    $('setupGoSettings').addEventListener('click', function () { showView('settings'); });
    $('addRemoteBtn').addEventListener('click', function () { openProfileEditor(null); });
    $('searchInput').addEventListener('input', function () { state.search = this.value; renderHome(); });
    $('remoteBack').addEventListener('click', function () { showView('home'); });
    $('remoteFav').addEventListener('click', function () {
      var p = currentProfile(); if (!p) return;
      p.favorite = !p.favorite;
      saveDb().then(function () { renderRemote(); renderHome(); }).catch(function () {});
    });
    $('remoteMenu').addEventListener('click', function () {
      var p = currentProfile(); if (p) openRemoteMenu(p);
    });
    $('doneEditBtn').addEventListener('click', function () { state.editing = false; renderRemote(); });
    $('addButtonBtn').addEventListener('click', function () {
      var p = currentProfile(); if (p) openButtonEditor(p, null);
    });
    document.addEventListener('keydown', function (ev) {
      if (ev.key === 'Escape' && !$('sheet').classList.contains('hidden')) closeSheet();
    });
    $('sheetCancel').addEventListener('click', closeSheet);
    $('sheetSave').addEventListener('click', function () {
      if (state.sheetOnSave) state.sheetOnSave();
    });
    $('setupForm').addEventListener('submit', submitSetup);
    $('configForm').addEventListener('submit', function (ev) { ev.preventDefault(); submitConfig(); });
    $('noticeRestartBtn').addEventListener('click', confirmRestart);
    $('restartBtn').addEventListener('click', confirmRestart);
    $('factoryBtn').addEventListener('click', confirmFactory);
    $('diagRefresh').addEventListener('click', function () {
      api('/api/status').then(function (s) { state.status = s; renderDiagnostics(); renderStatusPill(); })
        .catch(function (e) { toast(e.message || t('errNetwork'), true); });
    });
    $('exportBtn').addEventListener('click', exportBackup);
    $('restoreBtn').addEventListener('click', function () { $('restoreFile').click(); });
    $('restoreFile').addEventListener('change', function () {
      var f = this.files && this.files[0]; if (!f) return;
      var reader = new FileReader();
      reader.onload = function () {
        var d = null;
        try { d = JSON.parse(reader.result); } catch (e) { toast(t('backupInvalid'), true); return; }
        applyBackup(d);
      };
      reader.readAsText(f);
      this.value = '';
    });
    $('importProfileBtn').addEventListener('click', function () { $('importProfileFile').click(); });
    $('importProfileFile').addEventListener('change', function () {
      var f = this.files && this.files[0]; if (!f) return;
      var reader = new FileReader();
      reader.onload = function () {
        var d = null;
        try { d = JSON.parse(reader.result); } catch (e) { toast(t('profileImportInvalid'), true); return; }
        importProfile(d);
      };
      reader.readAsText(f);
      this.value = '';
    });
    $('otaFile').addEventListener('change', chooseOtaFile);
    $('otaStart').addEventListener('click', startOta);
    $('learnStart').addEventListener('click', advancedLearnStart);
    $('learnCancel').addEventListener('click', function () {
      stopLearningPolling();
      $('learnProgress').classList.add('hidden');
      $('learnResult').textContent = t('learnInvalid');
      $('learnResult').classList.remove('hidden');
      post('/api/learn/cancel', {}).catch(function () {});
    });
    $('learnSave').addEventListener('click', advancedLearnSave);
  }

  // ---------------------------------------------------------------------
  // Boot
  // ---------------------------------------------------------------------
  try {
    var savedTheme = localStorage.getItem('rovixane-theme');
    if (savedTheme === 'light' || savedTheme === 'dark') state.theme = savedTheme;
    var savedLang = localStorage.getItem('rovixane-lang');
    if (savedLang === 'bn' || savedLang === 'en') state.lang = savedLang;
  } catch (e) { /* storage unavailable */ }

  applyTheme(state.theme);
  applyI18n();
  bind();
  loadAll();
  setInterval(function () {
    api('/api/status').then(function (s) {
      state.status = s;
      renderStatusPill();
      if (state.view === 'settings') renderDiagnostics();
    }).catch(function () {});
  }, 6000);
})();
