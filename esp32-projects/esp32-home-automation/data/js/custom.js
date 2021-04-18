// Locate the 3 checkbox
const tube = document.querySelector('#tube');
const bulb = document.querySelector('#bulb');
const socket = document.querySelector('#socket');

// Add event Listener to the 3 checkbox
tube.addEventListener('change', toggleStatus);
bulb.addEventListener('change', toggleStatus);
socket.addEventListener('change', toggleStatus);

async function sendRequestToServer(url) {
  try {
    let res = await fetch(url);
    return await res.json();
  } catch (error) {
    console.log(error);
  }
}

async function toggleStatus(e) {
  let sourceElementName = e.target.name;
  let url = '/toggle/' + sourceElementName + '?status=';
  if (e.target.checked) {
    url += 'true';
  } else {
    url += 'false';
  }
  console.log("Sending to " + url);

  let response = await sendRequestToServer(url);

  console.log(response);
}

const slider = document.querySelector("#dim-value-slider");
const output = document.querySelector("#dim-value");
// Add event on range change
slider.addEventListener('input', updateDimmerValue);

async function updateDimmerValue(e) {
  //send request
  let url = '/dimmer/change' + '?value=' + slider.value;
  console.log("Sending to " + url);

  let response = await sendRequestToServer(url);

  console.log(response);

  //update dimmer value
  output.innerHTML = slider.value;
}



