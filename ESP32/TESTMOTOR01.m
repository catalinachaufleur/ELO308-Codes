% #define ain1  16
% #define ain2  27
% #define pwm_a  17
% 
% 
% pinMode(ain1, OUTPUT); 
% pinMode(ain2, OUTPUT); 
% analogWriteFrequency(500);
% 
% digitalWrite(ain1, HIGH); 
% digitalWrite(ain2, LOW);
% analogWrite(pwm_a, Velocidad_motor_der,resolucion); 
% 
% configurePin(a,'D27','DigitalOutput');
% pinMode = configurePin(a,'D27');
% 
% configurePin(a,'D16','DigitalOutput');
% pinMode = configurePin(a,'D16');
% 
% configurePin(a,'D17','PWM');
% pinMode = configurePin(a,'D17');
% 
% for x =0:4
%     writeDigitalPin(a,'D27',0);
%     writeDigitalPin(a,'D16',1);
%     writePWMDutyCycle(a,'D17',1);
%     pause(1);
%     writePWMDutyCycle(a,'D17',0);
% end


% 
% ip = '192.168.1.20'; % Dirección IP de la ESP32
% puerto = 80; % Puerto utilizado en la ESP32
% esp32 = tcpclient(ip, puerto);

esp32 = tcpclient('10.2.51.225', 1234);
fopen(esp32);
fprintf(esp32, 'codigo1\n');  % Envía el comando "codigo1"
response = fscanf(esp32);  % Lee la respuesta del ESP32
disp(response)
read(esp32)

for x =0:10  
    response = fscanf(esp32);  % Lee la respuesta del ESP32
    disp(response)
    pause(0.25);
end

clear esp32;


