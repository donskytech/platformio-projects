let statusDiv = document.querySelector("#statusDiv");
if (!!window.EventSource) {
  var source = new EventSource("/events");

  source.addEventListener(
    "open",
    function (e) {
      console.log("Events Connected");
    },
    false
  );

  source.addEventListener(
    "error",
    function (e) {
      if (e.target.readyState != EventSource.OPEN) {
        console.log("Events Disconnected");
      }
    },
    false
  );

  source.addEventListener(
    "message",
    function (e) {
      console.log("message", e.data);
    },
    false
  );

  source.addEventListener(
    "noFingerprint",
    function (e) {
      console.log(e.data);
      showMessage("message-info", e.data);
    },
    false
  );
  source.addEventListener(
    "accessGranted",
    function (e) {
      console.log(e.data);
      showMessage("message-granted", e.data);
    },
    false
  );
  source.addEventListener(
    "accessDenied",
    function (e) {
      console.log(e.data);
      showMessage("message-denied", e.data);
    },
    false
  );
  source.addEventListener(
    "unknownError",
    function (e) {
      console.log("Unknown Error", e.data);
      showMessage("message-error", e.data);
    },
    false
  );
}

function showMessage(newClass, message) {
  // clear the classes
  statusDiv.removeAttribute("class");
  // adding multiple class
  statusDiv.classList.add("message", newClass);
  //   Change message
  statusDiv.innerHTML = message;
}
