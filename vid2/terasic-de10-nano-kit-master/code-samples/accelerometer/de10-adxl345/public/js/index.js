var trace1 = {
  x: [0],
  y: [0.0],
  mode: 'lines',
  line: {
      shape: "spline",
      smoothing: 0.5
  },
  name: 'X'
};

var trace2 = {
  x: [0],
  y: [0.0],
  mode: 'lines',
  line: {
      shape: "spline",
      smoothing: 0.5
  },
  name: 'Y'
};

var trace3 = {
  x: [0],
  y: [0.0],
  mode: 'lines',
  line: {
      shape: "spline",
      smoothing: 0.5
  },
  name: 'Z'
};

var data = [trace1, trace2, trace3];

var layout = {
  title: 'ADXL345 live data for past 60 seconds',
  xaxis: {
    title: 'Time (s)'
  },
  yaxis: {
    title: 'Force (g)'
  }
};

Plotly.newPlot('myDiv', data, layout);

window.WebSocket = window.WebSocket || window.MozWebSocket;

var connection = new WebSocket('ws://192.168.1.10:3001'); // Change to match your own DE10-Nano IP
var messagesPerSecond = 10; // Change to match your server data rate
var messagesReceived = 0;

connection.onopen = function () {
    // Connection is open
    console.log('Connection to websocket established...');
};

connection.onerror = function (error) {
    // Data error
    console.log('Error: ' + error.data);
};

connection.onmessage = function (message) {
    // Parse data and push data to plot
    try {
        var adxlData = JSON.parse(message.data); // Deserialize incoming JSON acceleration data

        // Divide the X axis by the number of messages received per second, so that major units are elapsed seconds
        var ts = messagesReceived/messagesPerSecond;

        var newData = {
            x: [[ts], [ts], [ts]],
            y: [[adxlData.x], [adxlData.y], [adxlData.z]]
        }

        // Extend the current graph, last integer here is the number of X values to keep before discarding old data
        Plotly.extendTraces('myDiv', newData, [0, 1, 2], 60 * messagesPerSecond);
        messagesReceived++;
    } catch (e) {
        console.log('This doesn\'t look like valid JSON: ', message.data);
        return;
    }
};
