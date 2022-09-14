function toggleButtonSwitch(e) {
  var switchButton = document.getElementById("switch");
  
  var toggleValue = "";
  if (switchButton.checked) {
    console.log("On!");
    toggleValue = "ON";
  } else {
    console.log("Off!");
    toggleValue = "OFF"
  }
  fetch( `/toggle?status=${toggleValue}`)
  .then( response => {
    console.log(response);
  } )
}
