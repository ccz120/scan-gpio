
sudo chmod +x service/scan-gpio.sh
sudo cp service/scan-gpio.sh /usr/bin/scan-gpio.sh
sudo cp service/scan-gpio.service /etc/systemd/system/scan-gpio.service

echo "enable & start service"
sudo systemctl enable scan-gpio.service
sudo systemctl start scan-gpio.service

echo "finish, if you see green text all is ok"
sudo systemctl status scan-gpio.service
