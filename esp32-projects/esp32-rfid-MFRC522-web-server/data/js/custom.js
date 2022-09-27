window.addEventListener("load", onLoad);

function onLoad() {
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
        // console.log("message", e.data);
        const message = JSON.parse(e.data);
        console.log(message);
        let accessId = document.getElementById("access-id");
        accessId.innerHTML = message.rfid_tag_id;
        accessId.style.display = 'block';

        let accessDenied = document.getElementById("access-denied")
        let accessApproved = document.getElementById("access-approve")

        let status = message.status;
        if(status){
          accessDenied.style.display = 'none';
          accessApproved.style.display = 'block';
        }else{
          accessDenied.style.display = 'block';
          accessApproved.style.display = 'none';
        }
      },
      false
    );

    source.addEventListener(
      "myevent",
      function (e) {
        console.log("myevent", e.data);
      },
      false
    );
  }
}
