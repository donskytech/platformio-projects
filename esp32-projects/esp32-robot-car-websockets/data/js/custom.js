var targetUrl = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onLoad);


function onLoad() {
  initializeSocket();
}

function initializeSocket() {
  console.log('Opening WebSocket connection to ESP32...');
  websocket = new WebSocket(targetUrl);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}
function onOpen(event) {
  console.log('Starting connection to server..');
}
function onClose(event) {
  console.log('Closing connection to server..');
  setTimeout(initializeSocket, 2000);
}
function onMessage(event) {
  console.log("WebSocket message received:", event)
}

function sendMessage(message) {
  websocket.send(message);
}

/*
Speed Settings Handler
*/
var speedSettings = document.querySelectorAll(
  'input[type=radio][name="speed-settings"]'
);
speedSettings.forEach((radio) =>
  radio.addEventListener("change", () => 
  {
    var speedSettings = radio.value;
    console.log('Speed Settings :: ' + speedSettings)
    sendMessage(speedSettings); 
  }
));


/*
O-Pad/ D-Pad Controller and Javascript Code
*/
// Prevent scrolling on every click!
// super sweet vanilla JS delegated event handling!
document.body.addEventListener("click", function (e) {
  if (e.target && e.target.nodeName == "A") {
    e.preventDefault();
  }
});

function touchStartHandler(event) {
  var direction = event.target.dataset.direction;
  console.log('Touch Start :: ' + direction)
  sendMessage(direction);
}

function touchEndHandler(event) {
  const stop_command = 'stop';
  var direction = event.target.dataset.direction;
  console.log('Touch End :: ' + direction)
  sendMessage(stop_command);
}


document.querySelectorAll('.control').forEach(item => {
  item.addEventListener('touchstart', touchStartHandler)
})

document.querySelectorAll('.control').forEach(item => {
  item.addEventListener('touchend', touchEndHandler)
})




