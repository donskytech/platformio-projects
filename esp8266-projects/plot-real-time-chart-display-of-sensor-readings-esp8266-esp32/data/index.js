// Declare variables
var targetUrl = `ws://${window.location.hostname}/ws`;
var websocket;
var ctx;
var myChart;
const MAX_DATA_COUNT = 20;

// Add function for body load
window.addEventListener("load", onLoad);

// Function to run during on load of the HTML Page
function onLoad() {
  initializeSocket();
  initializeChart();
}

// Initialize the chart
function initializeChart(){
  ctx = document.getElementById("myChart").getContext("2d");
  myChart = new Chart(ctx, {
    type: "line",
    data: {
      datasets: [{ label: "LDR" }],
    },
    options: {
      borderWidth: 3,
      borderColor: ["rgba(255, 99, 132, 1)"],
    },
  });
}

// Initialize the Websockets
function initializeSocket() {
  console.log(
    `Opening WebSocket connection to Microcontroller :: ${targetUrl}`
  );
  websocket = new WebSocket(targetUrl);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}
// Websocket Callback Client function 
function onOpen(event) {
  console.log("Starting connection to server..");
}
function onClose(event) {
  console.log("Closing connection to server..");
  setTimeout(initializeSocket, 2000);
}
function onMessage(event) {
  console.log("WebSocket message received:", event);
  // Show only MAX_DATA_COUNT data
  if (myChart.data.labels.length > MAX_DATA_COUNT) {
    removeFirstData();
  }
  addData(getCurrentDateTime(), event.data);
}

// Get the current date time.  This will serve as the x-axis of our sensor data
function getCurrentDateTime() {
  var today = new Date();
  var date =
    today.getFullYear() + "-" + (today.getMonth() + 1) + "-" + today.getDate();
  var time =
    today.getHours() + ":" + today.getMinutes() + ":" + today.getSeconds();
  var dateTime = date + " " + time;
  return dateTime;
}

// add sensor data to chart
function addData(label, data) {
  myChart.data.labels.push(label);
  myChart.data.datasets.forEach((dataset) => {
    dataset.data.push(data);
  });
  myChart.update();
}

// Remove the first data
function removeFirstData() {
  myChart.data.labels.splice(0, 1);
  myChart.data.datasets.forEach((dataset) => {
    dataset.data.shift();
  });
}
