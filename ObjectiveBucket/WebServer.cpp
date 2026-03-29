#include "WebServer.h"

static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0">
<title>Objective Bucket</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=Barlow+Condensed:wght@400;600;800&family=Barlow:wght@400;500&display=swap');

  :root {
    --bg: #0d0f12;
    --surface: #171a1f;
    --border: #2a2d35;
    --accent: #f5a623;
    --green: #3ddc84;
    --red: #ff4d4d;
    --text: #e8eaf0;
    --muted: #6b7280;
    --radius: 10px;
  }

  * { box-sizing: border-box; margin: 0; padding: 0; }

  body {
    background: var(--bg);
    color: var(--text);
    font-family: 'Barlow', sans-serif;
    min-height: 100vh;
    padding: 0 0 40px 0;
  }

  header {
    background: var(--surface);
    border-bottom: 2px solid var(--accent);
    padding: 16px 20px 12px;
    display: flex;
    align-items: baseline;
    gap: 10px;
  }
  header h1 {
    font-family: 'Barlow Condensed', sans-serif;
    font-weight: 800;
    font-size: 26px;
    color: var(--accent);
    text-transform: uppercase;
  }
  header .subtitle {
    font-size: 12px;
    color: var(--muted);
    text-transform: uppercase;
    letter-spacing: 1px;
  }

  .status-bar {
    background: var(--surface);
    border-bottom: 1px solid var(--border);
    padding: 8px 20px;
    display: flex;
    align-items: center;
    gap: 8px;
    font-size: 13px;
    color: var(--muted);
  }
  .status-dot {
    width: 8px; height: 8px;
    border-radius: 50%;
    flex-shrink: 0;
    transition: background 0.3s;
  }
  .status-dot.idle    { background: var(--muted); }
  .status-dot.running { background: var(--green); box-shadow: 0 0 6px var(--green); }
  .status-dot.paused  { background: var(--accent); box-shadow: 0 0 6px var(--accent); }
  .status-label { color: var(--text); font-weight: 500; }

  main { padding: 20px 16px 0; max-width: 480px; margin: 0 auto; }

  .section-label {
    font-family: 'Barlow Condensed', sans-serif;
    font-weight: 600;
    font-size: 11px;
    letter-spacing: 2px;
    text-transform: uppercase;
    color: var(--muted);
    margin-bottom: 10px;
  }

  /* Game list */
  .game-list { display: flex; flex-direction: column; gap: 8px; margin-bottom: 24px; }
  .game-item {
    background: var(--surface);
    border: 1px solid var(--border);
    border-radius: var(--radius);
    padding: 16px;
    cursor: pointer;
    display: flex;
    align-items: center;
    gap: 12px;
    -webkit-tap-highlight-color: transparent;
  }
  .game-item:active { background: #1e2229; }
  .game-item.selected { border-color: var(--accent); background: rgba(245,166,35,0.07); }
  .game-bullet {
    width: 10px; height: 10px;
    border-radius: 50%;
    border: 2px solid var(--border);
    flex-shrink: 0;
    transition: border-color 0.15s, background 0.15s;
  }
  .game-item.selected .game-bullet { border-color: var(--accent); background: var(--accent); }
  .game-name {
    font-family: 'Barlow Condensed', sans-serif;
    font-weight: 600;
    font-size: 20px;
  }

  /* Config view */
  #config-view { display: none; }
  .config-header { display: flex; align-items: center; gap: 10px; margin-bottom: 20px; }
  .back-btn {
    background: none;
    border: 1px solid var(--border);
    color: var(--muted);
    border-radius: 6px;
    padding: 6px 12px;
    font-family: 'Barlow', sans-serif;
    font-size: 13px;
    cursor: pointer;
  }
  .config-title {
    font-family: 'Barlow Condensed', sans-serif;
    font-weight: 800;
    font-size: 22px;
    text-transform: uppercase;
  }
  .config-list { display: flex; flex-direction: column; gap: 16px; margin-bottom: 28px; }
  .config-item {
    background: var(--surface);
    border: 1px solid var(--border);
    border-radius: var(--radius);
    padding: 16px;
  }
  .config-item label {
    display: flex;
    justify-content: space-between;
    align-items: baseline;
    font-size: 14px;
    font-weight: 500;
    margin-bottom: 12px;
  }
  .config-value {
    font-family: 'Barlow Condensed', sans-serif;
    font-weight: 600;
    font-size: 20px;
    color: var(--accent);
  }
  input[type=range] {
    -webkit-appearance: none;
    width: 100%; height: 4px;
    border-radius: 2px;
    background: var(--border);
    outline: none;
  }
  input[type=range]::-webkit-slider-thumb {
    -webkit-appearance: none;
    width: 22px; height: 22px;
    border-radius: 50%;
    background: var(--accent);
    cursor: pointer;
    box-shadow: 0 0 0 3px rgba(245,166,35,0.2);
  }

  /* In-game view */
  #game-view { display: none; }
  .game-header {
    display: flex;
    align-items: center;
    justify-content: space-between;
    margin-bottom: 20px;
  }
  .game-title {
    font-family: 'Barlow Condensed', sans-serif;
    font-weight: 800;
    font-size: 24px;
    text-transform: uppercase;
  }

  /* Buttons */
  .btn {
    display: block; width: 100%;
    padding: 16px;
    border: none;
    border-radius: var(--radius);
    font-family: 'Barlow Condensed', sans-serif;
    font-weight: 800;
    font-size: 20px;
    letter-spacing: 1px;
    text-transform: uppercase;
    cursor: pointer;
    transition: opacity 0.15s, transform 0.1s;
    -webkit-tap-highlight-color: transparent;
    margin-bottom: 10px;
  }
  .btn:active { opacity: 0.85; transform: scale(0.98); }
  .btn:disabled { opacity: 0.35; pointer-events: none; }
  .btn-primary   { background: var(--accent); color: #111; }
  .btn-secondary { background: var(--surface); color: var(--text); border: 1px solid var(--border); }
  .btn-danger    { background: var(--red);    color: #fff; }
  .btn-green     { background: var(--green);  color: #111; }

  .divider { border: none; border-top: 1px solid var(--border); margin: 20px 0; }

  .toast {
    position: fixed;
    bottom: 24px; left: 50%;
    transform: translateX(-50%) translateY(80px);
    background: #1e2229;
    border: 1px solid var(--border);
    border-radius: 8px;
    padding: 10px 20px;
    font-size: 14px;
    color: var(--text);
    transition: transform 0.25s ease;
    pointer-events: none;
    white-space: nowrap;
    z-index: 100;
  }
  .toast.show { transform: translateX(-50%) translateY(0); }
</style>
</head>
<body>

<header>
  <h1>Objective Bucket</h1>
  <span class="subtitle">Derby Darts</span>
</header>

<div class="status-bar">
  <div class="status-dot idle" id="statusDot"></div>
  <span id="statusLabel" class="status-label">Idle</span>
</div>

<main>
  <!-- GAME SELECT -->
  <div id="game-select-view">
    <p class="section-label">Select Game Mode</p>
    <div class="game-list" id="gameList"></div>
    <button class="btn btn-primary" id="btnConfigure">Configure &amp; Start</button>
  </div>

  <!-- CONFIG -->
  <div id="config-view">
    <div class="config-header">
      <button class="back-btn" id="btnBack">&#8592; Back</button>
      <span class="config-title" id="configTitle"></span>
    </div>
    <div class="config-list" id="configList"></div>
    <button class="btn btn-primary" id="btnStart">Start Game</button>
  </div>

  <!-- IN-GAME -->
  <div id="game-view">
    <div class="game-header">
      <span class="game-title" id="gameViewTitle"></span>
    </div>
    <button class="btn btn-green"     id="btnResume" style="display:none">Resume</button>
    <button class="btn btn-secondary" id="btnPause">Pause</button>
    <button class="btn btn-secondary" id="btnReset">Reset</button>
    <hr class="divider">
    <button class="btn btn-danger"    id="btnExit">Exit Game</button>
  </div>
</main>

<div class="toast" id="toast"></div>

<script>
  let state = { running: false, paused: false, mode: 0, games: [], configs: [] };
  let pendingMode = 0;
  let pendingConfigs = {};

  const views = {
    select: document.getElementById('game-select-view'),
    config: document.getElementById('config-view'),
    game:   document.getElementById('game-view'),
  };

  function showView(name) {
    Object.values(views).forEach(v => v.style.display = 'none');
    views[name].style.display = '';
  }

  function updateStatusBar() {
    const dot   = document.getElementById('statusDot');
    const label = document.getElementById('statusLabel');
    dot.className = 'status-dot';
    if (!state.running) {
      dot.classList.add('idle');
      label.textContent = 'Idle — select a game';
    } else if (state.paused) {
      dot.classList.add('paused');
      label.textContent = 'Paused — ' + (state.games[state.mode] || '');
    } else {
      dot.classList.add('running');
      label.textContent = 'Running — ' + (state.games[state.mode] || '');
    }
  }

  function renderGameList() {
    const list = document.getElementById('gameList');
    list.innerHTML = '';
    state.games.forEach((name, i) => {
      const el = document.createElement('div');
      el.className = 'game-item' + (i === pendingMode ? ' selected' : '');
      el.innerHTML = `<div class="game-bullet"></div><span class="game-name">${name}</span>`;
      el.addEventListener('click', () => {
        pendingMode = i;
        post('/select', { index: i });
        renderGameList();
      });
      list.appendChild(el);
    });
  }

  function fmtSec(s) {
    const m = Math.floor(s / 60);
    const sec = s % 60;
    return m + ':' + String(sec).padStart(2, '0');
  }

  function renderConfigView() {
    pendingConfigs = {};
    document.getElementById('configTitle').textContent = state.games[state.mode] || '';
    const list = document.getElementById('configList');
    list.innerHTML = '';

    if (!state.configs || state.configs.length === 0) {
      list.innerHTML = '<p style="color:var(--muted);font-size:14px;">No settings for this mode.</p>';
      return;
    }

    state.configs.forEach(cfg => {
      pendingConfigs[cfg.key] = cfg.value;
      const displayVal = cfg.unit === 'sec' ? fmtSec(cfg.value) : cfg.value + ' ' + cfg.unit;
      const item = document.createElement('div');
      item.className = 'config-item';
      item.innerHTML = `
        <label>
          <span>${cfg.label}</span>
          <span class="config-value" id="val-${cfg.key}">${displayVal}</span>
        </label>
        <input type="range" id="range-${cfg.key}"
          min="${cfg.min}" max="${cfg.max}" value="${cfg.value}"
          step="${cfg.unit === 'sec' ? 30 : 1}">
      `;
      list.appendChild(item);
      const range = item.querySelector('input');
      const valEl = item.querySelector('.config-value');
      range.addEventListener('input', () => {
        const v = parseInt(range.value);
        pendingConfigs[cfg.key] = v;
        valEl.textContent = cfg.unit === 'sec' ? fmtSec(v) : v + ' ' + cfg.unit;
      });
    });
  }

  function renderGameView() {
    document.getElementById('gameViewTitle').textContent = state.games[state.mode] || '';
    document.getElementById('btnPause').style.display  = state.paused ? 'none' : '';
    document.getElementById('btnResume').style.display = state.paused ? '' : 'none';
  }

  function render() {
    updateStatusBar();
    if (state.running) { renderGameView(); showView('game'); }
    else               { renderGameList(); showView('select'); }
  }

  async function post(path, body) {
    try {
      const r = await fetch(path, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(body || {}),
      });
      if (!r.ok) showToast('Error: ' + r.status);
    } catch(e) { showToast('Connection lost'); }
  }

  async function fetchState() {
    try {
      const r = await fetch('/state');
      if (!r.ok) return;
      const data = await r.json();
      const wasRunning = state.running;
      state = data;
      if (!wasRunning && data.running)       { renderGameView(); showView('game'); updateStatusBar(); }
      else if (wasRunning && !data.running)  { render(); }
      else if (data.running)                 { renderGameView(); updateStatusBar(); }
      else                                   { updateStatusBar(); }
    } catch(e) {}
  }

  document.getElementById('btnConfigure').addEventListener('click', () => {
    renderConfigView(); showView('config');
  });
  document.getElementById('btnBack').addEventListener('click', () => showView('select'));
  document.getElementById('btnStart').addEventListener('click', async () => {
    for (const [key, value] of Object.entries(pendingConfigs)) {
      await post('/config', { key, value });
    }
    await post('/start');
    showToast('Game starting!');
  });
  document.getElementById('btnPause').addEventListener('click', () => {
    post('/pause'); state.paused = true; renderGameView(); updateStatusBar();
  });
  document.getElementById('btnResume').addEventListener('click', () => {
    post('/resume'); state.paused = false; renderGameView(); updateStatusBar();
  });
  document.getElementById('btnReset').addEventListener('click', () => {
    post('/reset'); showToast('Game reset');
  });
  document.getElementById('btnExit').addEventListener('click', async () => {
    await post('/exit'); showToast('Exiting game...');
  });

  function showToast(msg) {
    const t = document.getElementById('toast');
    t.textContent = msg;
    t.classList.add('show');
    setTimeout(() => t.classList.remove('show'), 2000);
  }

  render();
  setInterval(fetchState, 1000);
</script>
</body>
</html>
)rawliteral";

// ---------------------------------------------------------------------------

OBWebServer::OBWebServer(GameModeRunner *runner) : server(80), runner(runner) {}

void OBWebServer::Begin()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(AP_IP_ADDR, AP_IP_ADDR, IPAddress(255, 255, 255, 0));
  WiFi.softAP(AP_SSID);

  Serial.print("AP started. IP: ");
  Serial.println(WiFi.softAPIP());

  if (MDNS.begin(AP_HOSTNAME)) {
    Serial.println("mDNS started: http://ob.local");
    MDNS.addService("http", "tcp", 80);
  } else {
    Serial.println("mDNS failed — use 192.168.4.1");
  }

  setupRoutes();
  server.begin();
  Serial.println("HTTP server started");
}

void OBWebServer::Update()
{
  MDNS.update();
}

String OBWebServer::buildStateJson()
{
  String json = "{";
  json += "\"running\":" + String(runner->IsRunning() ? "true" : "false") + ",";
  json += "\"paused\":"  + String(runner->IsPaused()  ? "true" : "false") + ",";
  json += "\"mode\":"    + String(runner->CurrentMode()) + ",";

  json += "\"games\":[";
  for (int i = 0; i < runner->GetNumGames(); i++) {
    if (i > 0) json += ",";
    json += "\"" + runner->GetGame(i)->GetName() + "\"";
  }
  json += "],";

  json += "\"configs\":[";
  int mode = runner->CurrentMode();
  if (mode != GAMEMODE_IDLE && mode < runner->GetNumGames()) {
    GameMode *g = runner->GetGame(mode);
    for (int i = 0; i < g->GetConfigCount(); i++) {
      GameConfig c = g->GetConfig(i);
      if (i > 0) json += ",";
      json += "{";
      json += "\"key\":\""   + c.key      + "\",";
      json += "\"label\":\"" + c.label    + "\",";
      json += "\"value\":"   + String(c.value)    + ",";
      json += "\"min\":"     + String(c.minValue) + ",";
      json += "\"max\":"     + String(c.maxValue) + ",";
      json += "\"unit\":\""  + c.unit     + "\"";
      json += "}";
    }
  }
  json += "]}";
  return json;
}

void OBWebServer::setupRoutes()
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *req) {
    req->send_P(200, "text/html", INDEX_HTML);
  });

  server.on("/state", HTTP_GET, [this](AsyncWebServerRequest *req) {
    req->send(200, "application/json", buildStateJson());
  });

  server.on("/select", HTTP_POST, [this](AsyncWebServerRequest *req){}, NULL,
    [this](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t) {
      String body = String((char*)data).substring(0, len);
      int idx = body.indexOf("\"index\"");
      if (idx >= 0) runner->WebSelectGame(body.substring(body.indexOf(':', idx) + 1).toInt());
      req->send(200, "application/json", "{\"ok\":true}");
    });

  server.on("/start", HTTP_POST, [this](AsyncWebServerRequest *req){}, NULL,
    [this](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t) {
      runner->WebStartGame();
      req->send(200, "application/json", "{\"ok\":true}");
    });

  server.on("/reset", HTTP_POST, [this](AsyncWebServerRequest *req){}, NULL,
    [this](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t) {
      runner->WebResetGame();
      req->send(200, "application/json", "{\"ok\":true}");
    });

  server.on("/pause", HTTP_POST, [this](AsyncWebServerRequest *req){}, NULL,
    [this](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t) {
      runner->WebPauseGame();
      req->send(200, "application/json", "{\"ok\":true}");
    });

  server.on("/resume", HTTP_POST, [this](AsyncWebServerRequest *req){}, NULL,
    [this](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t) {
      runner->WebResumeGame();
      req->send(200, "application/json", "{\"ok\":true}");
    });

  server.on("/exit", HTTP_POST, [this](AsyncWebServerRequest *req){}, NULL,
    [this](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t) {
      if (runner->IsRunning()) {
        ButtonData fakeHold = { .event = ButtonState::Hold, .holdTime = 2000 };
        runner->ButtonEvent(ButtonEnum::ButtonReset, fakeHold);
      }
      req->send(200, "application/json", "{\"ok\":true}");
    });

  server.on("/config", HTTP_POST, [this](AsyncWebServerRequest *req){}, NULL,
    [this](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t) {
      String body = String((char*)data).substring(0, len);
      int keyIdx = body.indexOf("\"key\"");
      int valIdx = body.indexOf("\"value\"");
      if (keyIdx >= 0 && valIdx >= 0) {
        int qs  = body.indexOf('"', keyIdx + 5) + 1;
        int qe  = body.indexOf('"', qs);
        String key = body.substring(qs, qe);
        long value = body.substring(body.indexOf(':', valIdx) + 1).toInt();
        int mode = runner->CurrentMode();
        if (mode != GAMEMODE_IDLE && mode < runner->GetNumGames()) {
          runner->GetGame(mode)->SetConfig(key, value);
        }
      }
      req->send(200, "application/json", "{\"ok\":true}");
    });

  server.onNotFound([](AsyncWebServerRequest *req) {
    req->send(404, "text/plain", "Not found");
  });
}
