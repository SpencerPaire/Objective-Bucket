#include "WebServer.h"

static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Objective Bucket</title>
<style>
  body { font-family: sans-serif; background: #111; color: #eee; margin: 0; padding: 16px; max-width: 400px; margin: 0 auto; }
  h1   { color: #f5a623; margin: 16px 0 4px; }
  h2   { color: #aaa; font-size: 14px; font-weight: normal; margin: 0 0 24px; }

  .page { display: none; }
  .page.active { display: block; }

  .game-item {
    padding: 14px 16px;
    border: 1px solid #333;
    border-radius: 8px;
    margin-bottom: 8px;
    cursor: pointer;
    background: #1a1a1a;
  }
  .game-item.selected { border-color: #f5a623; background: #1f1a0f; }
  .game-name { font-size: 18px; font-weight: bold; }

  .timer-row {
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 16px;
    margin: 24px 0;
  }
  .timer-btn {
    font-size: 28px;
    width: 56px; height: 56px;
    background: #222;
    border: 1px solid #444;
    color: #eee;
    border-radius: 8px;
    cursor: pointer;
  }
  .timer-btn:disabled { opacity: 0.3; }
  .timer-val { font-size: 40px; font-weight: bold; color: #f5a623; min-width: 100px; text-align: center; }

  .countdown { font-size: 64px; font-weight: bold; text-align: center; color: #f5a623; margin: 24px 0; }
  .game-over { background: #8b0000; text-align: center; padding: 12px; border-radius: 8px; font-weight: bold; font-size: 20px; margin-bottom: 16px; display: none; }

  button.btn {
    display: block; width: 100%;
    padding: 16px; margin-bottom: 10px;
    border: none; border-radius: 8px;
    font-size: 16px; font-weight: bold;
    cursor: pointer;
  }
  .btn-start  { background: #f5a623; color: #111; }
  .btn-action { background: #222; color: #eee; border: 1px solid #444; }
  .btn-danger { background: #8b0000; color: #fff; }
  .btn-back   { background: none; color: #aaa; border: 1px solid #333; margin-bottom: 20px; padding: 8px 16px; border-radius: 6px; cursor: pointer; font-size: 14px; }

  hr { border: none; border-top: 1px solid #333; margin: 16px 0; }
</style>
</head>
<body>

<!-- PAGE: SELECT -->
<div class="page active" id="page-select">
  <h1>Objective Bucket</h1>
  <h2>Select a game mode</h2>
  <div id="game-list"></div>
  <button class="btn btn-start" onclick="goToConfigure()">Configure &amp; Start &#8594;</button>
</div>

<!-- PAGE: CONFIGURE -->
<div class="page" id="page-configure">
  <button class="btn-back" onclick="goToSelect()">&#8592; Back</button>
  <h1 id="cfg-title"></h1>
  <h2>Set match time</h2>
  <div class="timer-row">
    <button class="timer-btn" id="btn-minus" onclick="stepTime(-1)">&#8722;</button>
    <span class="timer-val" id="timer-display">6:00</span>
    <button class="timer-btn" id="btn-plus" onclick="stepTime(1)">+</button>
  </div>
  <p style="text-align:center; color:#666; font-size:13px; margin-bottom:24px;">Set to None for no timer.</p>
  <button class="btn btn-start" onclick="startGame()">Start Game</button>
</div>

<!-- PAGE: RUNNING -->
<div class="page" id="page-running">
  <h1 id="run-title"></h1>
  <div class="game-over" id="game-over-banner">GAME OVER</div>
  <div class="countdown" id="countdown"></div>
  <button class="btn btn-action" id="btn-pause-resume" onclick="togglePause()">Pause</button>
  <button class="btn btn-action" onclick="resetGame()">Reset</button>
  <hr>
  <button class="btn btn-danger" onclick="exitGame()">Exit Game</button>
</div>

<script>
var selectedMode   = 0;
var pendingTimeSec = 360;
var gameNames      = [];
var prevRunning    = false;
var prevPaused     = false;
var prevGameOver   = false;
var audioCtx       = null;

// Timer stepper: 0 = none, then 120..600 in steps of 30
var MIN_SEC = 0, MAX_SEC = 600, STEP = 30;

function fmt(s) {
  s = Math.max(0, Math.floor(s));
  var m = Math.floor(s / 60);
  var sec = s % 60;
  return m + ':' + (sec < 10 ? '0' : '') + sec;
}

function showPage(id) {
  document.querySelectorAll('.page').forEach(function(p) { p.classList.remove('active'); });
  document.getElementById(id).classList.add('active');
}

// ---- Buzzer sound (aggressive basketball buzzer) ----
function getCtx() {
  if (!audioCtx) audioCtx = new (window.AudioContext || window.webkitAudioContext)();
  return audioCtx;
}

function playBuzzer() {
  try {
    var ctx = getCtx();
    var now = ctx.currentTime;

    // Three blasts: 2s on, 1s off, 2s on, 1s off, 2s on
    [0, 3, 6].forEach(function(t) {
      var buzz = ctx.createOscillator();
      var gain = ctx.createGain();
      buzz.connect(gain);
      gain.connect(ctx.destination);
      buzz.type = 'square';
      buzz.frequency.value = 160;
      gain.gain.setValueAtTime(0.6, now + t);
      gain.gain.setValueAtTime(0.001, now + t + 2.0);
      buzz.start(now + t);
      buzz.stop(now + t + 2.0);
    });
  } catch(e) {}
}

document.body.addEventListener('touchstart', function() { getCtx(); }, { once: true });

// ---- Stepper ----
function stepperLabel(s) {
  if (s <= 0) return 'None';
  return fmt(s);
}

function updateStepper() {
  document.getElementById('timer-display').textContent = stepperLabel(pendingTimeSec);
  document.getElementById('btn-minus').disabled = (pendingTimeSec <= MIN_SEC);
  document.getElementById('btn-plus').disabled  = (pendingTimeSec >= MAX_SEC);
}

function stepTime(dir) {
  if (dir < 0) {
    // stepping down: 120 -> 0 (skip everything between)
    pendingTimeSec = (pendingTimeSec <= 30) ? 0 : pendingTimeSec - STEP;
  } else {
    // stepping up: 0 -> 120
    pendingTimeSec = (pendingTimeSec <= 0) ? 30 : Math.min(MAX_SEC, pendingTimeSec + STEP);
  }
  updateStepper();
}

// ---- Navigation ----
function goToSelect() { showPage('page-select'); }

function goToConfigure() {
  document.getElementById('cfg-title').textContent = gameNames[selectedMode] || '';
  updateStepper();
  showPage('page-configure');
  if (window.Notification && Notification.permission === 'default') Notification.requestPermission();
}

function startGame() {
  post('/config', { key: 'matchTime', value: pendingTimeSec }, function() {
    post('/start', {}, function() {
      document.getElementById('game-over-banner').style.display = 'none';
      document.getElementById('run-title').textContent = gameNames[selectedMode] || '';
      document.getElementById('btn-pause-resume').textContent = 'Pause';
      showPage('page-running');
    });
  });
}

// ---- In-game actions ----
function togglePause() {
  var btn = document.getElementById('btn-pause-resume');
  if (btn.textContent === 'Pause') {
    post('/pause', {}, function() { btn.textContent = 'Resume'; });
  } else {
    post('/resume', {}, function() {
      btn.textContent = 'Pause';
      document.getElementById('game-over-banner').style.display = 'none';
    });
  }
}

function resetGame() {
  post('/reset', {}, function() {
    document.getElementById('game-over-banner').style.display = 'none';
    document.getElementById('btn-pause-resume').textContent = 'Pause';
  });
}

function exitGame() {
  post('/exit', {}, function() {
    document.getElementById('game-over-banner').style.display = 'none';
    showPage('page-select');
  });
}

// ---- Game list ----
function renderGameList() {
  var list = document.getElementById('game-list');
  list.innerHTML = '';
  gameNames.forEach(function(name, i) {
    var el = document.createElement('div');
    el.className = 'game-item' + (i === selectedMode ? ' selected' : '');
    el.innerHTML = '<span class="game-name">' + name + '</span>';
    el.onclick = function() {
      selectedMode = i;
      post('/select', { index: i });
      renderGameList();
    };
    list.appendChild(el);
  });
}

// ---- XHR helpers ----
function post(path, body, cb) {
  var xhr = new XMLHttpRequest();
  xhr.open('POST', path, true);
  xhr.setRequestHeader('Content-Type', 'application/json');
  xhr.onload = function() { if (cb) cb(); };
  xhr.onerror = function() {};
  xhr.send(JSON.stringify(body || {}));
}

// ---- State poll ----
function poll() {
  var xhr = new XMLHttpRequest();
  xhr.open('GET', '/state', true);
  xhr.onload = function() {
    if (xhr.status !== 200) return;
    var s;
    try { s = JSON.parse(xhr.responseText); } catch(e) { return; }

    // Update game list
    if (JSON.stringify(s.games) !== JSON.stringify(gameNames)) {
      gameNames = s.games || [];
      renderGameList();
    }

    // Update countdown — hide if no timer (remainingMs == -1)
    var cd = document.getElementById('countdown');
    if (s.remainingMs < 0) {
      cd.style.display = 'none';
    } else {
      cd.style.display = '';
      cd.textContent = fmt(s.remainingMs / 1000);
    }

    // Game over — driven by server flag, not inferred
    if (s.gameOver && !prevGameOver) {
      document.getElementById('game-over-banner').style.display = 'block';
      document.getElementById('btn-pause-resume').textContent = 'Resume';
      playBuzzer();
      if (window.Notification && Notification.permission === 'granted') {
        new Notification('Game Over!', { body: 'The match has ended.' });
      }
    }
    // Clear banner when game resets (gameOver flag drops)
    if (!s.gameOver && prevGameOver) {
      document.getElementById('game-over-banner').style.display = 'none';
      document.getElementById('btn-pause-resume').textContent = 'Pause';
    }

    // Game exited from hardware
    if (prevRunning && !s.running) {
      document.getElementById('game-over-banner').style.display = 'none';
      showPage('page-select');
    }

    // Game started from hardware
    if (!prevRunning && s.running) {
      document.getElementById('run-title').textContent = gameNames[s.mode] || '';
      document.getElementById('btn-pause-resume').textContent = s.paused ? 'Resume' : 'Pause';
      showPage('page-running');
    }

    // Keep pause button in sync with hardware (only when not game-over)
    if (s.running && !s.gameOver) {
      document.getElementById('btn-pause-resume').textContent = s.paused ? 'Resume' : 'Pause';
    }

    prevRunning  = s.running;
    prevPaused   = s.paused;
    prevGameOver = s.gameOver;
  };
  xhr.onerror = function() {};
  xhr.send();
}

poll();
setInterval(poll, 1000);
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

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  if (MDNS.begin(AP_HOSTNAME)) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("mDNS: http://ob.local");
  } else {
    Serial.println("mDNS failed, use 192.168.4.1");
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
  String json = "";
  json.reserve(256);

  int  mode     = runner->CurrentMode();
  bool running  = runner->IsRunning();
  bool paused   = runner->IsPaused();
  int  nGames   = runner->GetNumGames();
  int  safeMode = (mode == GAMEMODE_IDLE || mode >= nGames) ? 0 : mode;

  bool gameOver = false;
  long remaining = -1;
  if (running && safeMode < nGames) {
    GameMode *g = runner->GetGame(safeMode);
    gameOver  = g->IsGameOver();
    remaining = g->GetRemainingMs();
  }

  json += "{\"running\":";   json += running  ? "true" : "false";
  json += ",\"paused\":";    json += paused   ? "true" : "false";
  json += ",\"gameOver\":";  json += gameOver ? "true" : "false";
  json += ",\"mode\":";      json += String(safeMode);
  json += ",\"games\":[";
  for (int i = 0; i < nGames; i++) {
    if (i > 0) json += ",";
    json += "\""; json += runner->GetGame(i)->GetName(); json += "\"";
  }
  json += "],\"remainingMs\":";
  json += String(remaining);
  json += "}";
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

  server.on("/select", HTTP_POST, [](AsyncWebServerRequest *req){}, NULL,
    [this](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t) {
      String body = String((char*)data).substring(0, len);
      int idx = body.indexOf("\"index\"");
      if (idx >= 0) runner->WebSelectGame(body.substring(body.indexOf(':', idx) + 1).toInt());
      req->send(200, "application/json", "{\"ok\":true}");
    });

  server.on("/start", HTTP_POST, [](AsyncWebServerRequest *req){}, NULL,
    [this](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t) {
      runner->WebStartGame();
      req->send(200, "application/json", "{\"ok\":true}");
    });

  server.on("/reset", HTTP_POST, [](AsyncWebServerRequest *req){}, NULL,
    [this](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t) {
      runner->WebResetGame();
      req->send(200, "application/json", "{\"ok\":true}");
    });

  server.on("/pause", HTTP_POST, [](AsyncWebServerRequest *req){}, NULL,
    [this](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t) {
      runner->WebPauseGame();
      req->send(200, "application/json", "{\"ok\":true}");
    });

  server.on("/resume", HTTP_POST, [](AsyncWebServerRequest *req){}, NULL,
    [this](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t) {
      runner->WebResumeGame();
      req->send(200, "application/json", "{\"ok\":true}");
    });

  server.on("/exit", HTTP_POST, [](AsyncWebServerRequest *req){}, NULL,
    [this](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t) {
      if (runner->IsRunning()) {
        ButtonData fakeHold = { .event = ButtonState::Hold, .holdTime = 2000 };
        runner->ButtonEvent(ButtonEnum::ButtonReset, fakeHold);
      }
      req->send(200, "application/json", "{\"ok\":true}");
    });

  server.on("/config", HTTP_POST, [](AsyncWebServerRequest *req){}, NULL,
    [this](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t) {
      String body = String((char*)data).substring(0, len);
      int keyIdx = body.indexOf("\"key\"");
      int valIdx = body.indexOf("\"value\"");
      if (keyIdx >= 0 && valIdx >= 0) {
        int qs     = body.indexOf('"', keyIdx + 5) + 1;
        int qe     = body.indexOf('"', qs);
        String key = body.substring(qs, qe);
        long value = body.substring(body.indexOf(':', valIdx) + 1).toInt();
        int mode   = runner->CurrentMode();
        int n      = runner->GetNumGames();
        int safe   = (mode == GAMEMODE_IDLE || mode >= n) ? 0 : mode;
        runner->GetGame(safe)->SetConfig(key, value);
      }
      req->send(200, "application/json", "{\"ok\":true}");
    });

  server.onNotFound([](AsyncWebServerRequest *req) {
    req->send(404, "text/plain", "Not found");
  });
}