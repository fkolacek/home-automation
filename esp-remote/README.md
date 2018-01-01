## ESP Sencor TV remote controller

Control your TV remotely with NodeMCU v3. It runs simple webserver and provides very simple API for sending control codes you can easily use it from command line like this:
```
TBD
```

It also comes with bundled WebUI (remote controller) so controling your TV is even easier (see below for example).

### NodeMCU v3 pinout
![nodemcu_v3_pinout](nodemcu_v3_pinout.png)

### Schema
![nodemcu_v3_schema](esp_remote.png)

### WebUI
It is located in `src/webui` directory. For using just edit the remote server variable and deploy it on some webserver.
![webui](remote_controller.png)


