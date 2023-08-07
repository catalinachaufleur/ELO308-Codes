% Ajustables
file_name = 'C:\Users\56975\Downloads\control_curvatura\control_curvatura\monitoreo2.csv'; % archivo csv

UDP_PORT = '1234';

% UDP Port
udp_port = udpport('LocalHost', '192.168.1.101', 'LocalPort', 1234);

% Creación de archivo CSV
header = 'Robot,Delta_muestra,Input_d,d_ref,vel_ref,Input_vel,Input_theta,Output_d,Output_vel,Output_theta,curvatura,vel_crucero,curvatura_predecesor,control';
fid = fopen(file_name, 'w');
fprintf(fid, '%s\n', header);
fclose(fid);

while true
    data = read(udp_port, udp_port.NumDatagramsAvailable); % leer datos
    testo = char(data');
    lista = strsplit(testo, ',');
    
    fid = fopen(file_name, 'a');
    fprintf(fid, '%s\n', testo);
    fclose(fid);
    
    disp(testo);
end

% Cerrar el puerto UDP al final
delete(udp_port);
clear udp_port;
