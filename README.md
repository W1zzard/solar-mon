# solar-mon
Project for monitoring solar charge controller

##docker
Docker folder contain all properties required to set up prometheus and grafana to get store and visualize info from solar controller.

##esp32
Contain source code of esp32 exporter, which export data from controller via modbus protocol and return to prometheus request.

##hardware
Contain some advices about hardware connection between esp32 and MPPT solar charge controller

#Usage
##prerequests
- averageValue library folder place into /libraries folder of arduino
- install libraries for wifi, and modbus

##deploy
1. edit prometheus_exporter.ino  ssid and password to set it for your wifi network
2. compile and deploy on esp32
3. edit `prometheus.yml` target ip address to address assigned to your arduino
4. run `docker-compose up` inside docker folder, login to grafana ip:3000 and create proper dashboard or import from `solar_dashboard.json`