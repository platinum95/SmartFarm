// Customize for your thingsboard instance
var TB_ADDRESS = "192.168.1.21"
var TB_PORT = 8080

//
// You need to replace `token` below with a JWT_TOKEN obtained from your
// thingsboard instance. Follow the instructions at the URL below, specifically
// the command at the end of the page beginning `curl -X POST ...`, which you
// must modify as appropriate (thingsboard IP address in particular):
//
//   https://thingsboard.io/docs/reference/rest-api/
//
var TB_TOKEN = "eyJhbGciOiJIUzUxMiJ9.eyJzdWIiOiJvbXVpcmdkQHRjZC5pZSIsInNjb3BlcyI6WyJURU5BTlRfQURNSU4iXSwidXNlcklkIjoiNjQ4ZjdkMDAtZWU1Yi0xMWU4LWJjMTYtNzdkNmI1MGUxYTY5IiwiZW5hYmxlZCI6dHJ1ZSwiaXNQdWJsaWMiOmZhbHNlLCJ0ZW5hbnRJZCI6IjVhMTliOTMwLWVlNWItMTFlOC1iYzE2LTc3ZDZiNTBlMWE2OSIsImN1c3RvbWVySWQiOiIxMzgxNDAwMC0xZGQyLTExYjItODA4MC04MDgwODA4MDgwODAiLCJpc3MiOiJ0aGluZ3Nib2FyZC5pbyIsImlhdCI6MTU0MzE4NzI4MCwiZXhwIjoxNTQzMTk0NDgwfQ.N8RY0Si_8uOmJT07HWU_tETcgVETer7C8Sqvk-lIHxo5gWInpfKyVJfeArOP0P-lDainRbIHzM23rVVK53uVOA";

// Create an array of thingsboard DEVICE IDs corresponding to your nRF52-DKs
// You can obtain these using COPY DEVICE ID in the thingsboard web UI
var DEVICE_IDS = [
    "8276aac0-ee8b-11e8-81df-470a083069a1"
];

// You might want to declare some constants to make it easier to identify
// your devices
var MY_BTN_DEVICE = 0;
var MY_LED_DEVICE = 0;

// Set the state of the lights on the device `deviceId`
function doLights(deviceId, lightNo, state) {

    // Use the server-side device RPC API to cause thingsboard to issue a device
    // RPC to a device that we identify by `buttonEntityId`
    // See: https://thingsboard.io/docs/user-guide/rpc/

    var request = require("request");
    var url = "http://" + TB_ADDRESS+":" + TB_PORT + "/api/plugins/rpc/oneway/" + deviceId;

    // The JSON RPC description must match that expected in tb_pubsub.c
    var req = {
        "method" : "putLights",
        "params" : {
            "ledno" : lightNo,
            "value" : state
        }
    };

    console.log("RPC Request: " + url + ": " + JSON.stringify(req));

    // Issue the HTTP POST request
    request({
        url: url,
        method: "POST",
        json: req,
        headers: {
            "X-Authorization": "Bearer " + TB_TOKEN,
            // Note the error in the TB docs: `Bearer` is missing from
            // `X-Authorization`, causing a 401 error response
        }
    }, function (error, response, body) {
        if (!error && response.statusCode === 200) {
            console.log("OK" + ((typeof body != 'undefined') ? ": " + body : ""));
        }
        else {
            console.log("error: " + error)
            console.log("response.statusCode: " + response.statusCode)
            console.log("response.statusText: " + response.statusText)
        }
    });
}

function doTest(deviceId, lightNo, state) {

    // Use the server-side device RPC API to cause thingsboard to issue a device
    // RPC to a device that we identify by `buttonEntityId`
    // See: https://thingsboard.io/docs/user-guide/rpc/

    var request = require("request");
    var url = "http://" + TB_ADDRESS+":" + TB_PORT + "/api/plugins/rpc/oneway/" + deviceId;

    // The JSON RPC description must match that expected in tb_pubsub.c
    var req = {
        "method" : "putTest",
        "params" : {
            "ledno" : lightNo,
            "value" : state
        }
    };

    console.log("RPC Request: " + url + ": " + JSON.stringify(req));
    console.log("doTest");
    // Issue the HTTP POST request
    request({
        url: url,
        method: "POST",
        json: req,
        headers: {
            "X-Authorization": "Bearer " + TB_TOKEN,
            // Note the error in the TB docs: `Bearer` is missing from
            // `X-Authorization`, causing a 401 error response
        }
    }, function (error, response, body) {
        if (!error && response.statusCode === 200) {
            console.log("OK" + ((typeof body != 'undefined') ? ": " + body : ""));
        }
        else {
            console.log("error: " + error)
            console.log("response.statusCode: " + response.statusCode)
            console.log("response.statusText: " + response.statusText)
        }
    });
}



// Process device telemetry updates received from thingsboard device `deviceId`
function processTelemetryData(deviceId, data) {

    // Note: Unfortunately the JSON parser gives us strings for the booleans
    // that we originally published from the tb_template device firmware. We
    // need to use string comparison to interpret them.

    console.log("Telemetry from " + deviceId + " : " + JSON.stringify(data));

    // Check that this is an update from the device we're interested in
    if (deviceId == DEVICE_IDS[MY_BTN_DEVICE]) {
        // Just check for an update to button state and mirror it in the
        // corresponding LED
        if (typeof data.btn0 !== 'undefined') {
            doTest(DEVICE_IDS[MY_LED_DEVICE], 3, data.btn0[0][1] == "true" ? true : false);
        }
        if (typeof data.btn1 !== 'undefined') {
            doLights(DEVICE_IDS[MY_LED_DEVICE], 2, data.btn1[0][1] == "true" ? true : false);
        }
        if (typeof data.btn2 !== 'undefined') {
            doLights(DEVICE_IDS[MY_LED_DEVICE], 1, data.btn2[0][1] == "true" ? true : false);
        }
        if (typeof data.btn3 !== 'undefined') {
            doLights(DEVICE_IDS[MY_LED_DEVICE], 0, data.btn3[0][1] == "true" ? true : false);
        }
    }
}


// Use the thingsboard Websocket API to subscribe to device telemetry updates
// See: https://thingsboard.io/docs/user-guide/telemetry/

var WebSocketClient = require('websocket').client;
var client = new WebSocketClient();

client.on('connectFailed', function(error) {
    console.log('Connect Error: ' + error.toString());
});

client.on('connect', function(connection) {

    console.log('WebSocket Client Connected');

    connection.on('error', function(error) {
        console.log("Connection Error: " + error.toString());
    });

    connection.on('close', function() {
        console.log('echo-protocol Connection Closed');
    });

    connection.on('message', function(message) {
        if (message.type === 'utf8') {
            var rxObj = JSON.parse(message.utf8Data);
            if (typeof rxObj.subscriptionId !== 'undefined') {
                processTelemetryData(DEVICE_IDS[rxObj.subscriptionId], rxObj.data);
            }
        }
    });

    // Subscribe to the latest telemetry from the device (specified by an index
    // into the DEVICE_IDS array)
    // See: https://thingsboard.io/docs/user-guide/telemetry/
    function subscribe(deviceIdx) {
        var req = {
            tsSubCmds: [
                {
                    entityType: "DEVICE",
                    entityId: DEVICE_IDS[deviceIdx],
                    scope: "LATEST_TELEMETRY",
                    cmdId: deviceIdx
                }
            ],
            historyCmds: [],
            attrSubCmds: []
        };

        console.log("Subscribing to " + DEVICE_IDS[deviceIdx]);
        connection.sendUTF(JSON.stringify(req));
    }

    if (connection.connected) {

        // Subscribe to telemetry updates for MY_BTN_DEVICE
        subscribe(MY_BTN_DEVICE);

        // Or subscribe to all if you want
        // for (deviceIdx = 0; deviceIdx < DEVICE_IDS.length; deviceIdx++) {
        //     subscribe(deviceIdx);
        // }
    }
});

client.connect("ws://" + TB_ADDRESS + ":" + TB_PORT + "/api/ws/plugins/telemetry?token=" + TB_TOKEN);
