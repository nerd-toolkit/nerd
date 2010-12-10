function plotMaxMotionDiff()

%max
file1 = '/home/fuchs/Desktop/Validation_Results/8/TraindataStatistics/Aimee_DynamixelModel_20091214_1131_standUp_Aimee_DynamixelModel_20091202_1213_0.000345449_2080.val.stat';
file2 = '/home/fuchs/Desktop/Validation_Results/8/TraindataStatistics/Aimee_DynamixelModel_20091214_1131_standUp_Aimee_DynamixelModel_20091202_1215_0.000497833_7613.val.stat';

%two qualitative best
file1 = '/home/fuchs/Desktop/Validation_Results/8/TraindataStatistics/Aimee_DynamixelModel_20091214_1131_standUp_Aimee_DynamixelModel_20091202_1209_0.000293757_9569.val.stat';
file2 = '/home/fuchs/Desktop/Validation_Results/8/TraindataStatistics/Aimee_DynamixelModel_20091214_1131_standUp_Aimee_DynamixelModel_20091202_1213_0.000345449_2080.val.stat';

%min
%file1 = '/home/fuchs/Desktop/Validation_Results/4/TraindataStatistics/Aimee_DynamixelModel_20091214_1123_standUp_Aimee_DynamixelModel_20091202_1248_0.000240485_8872.val.stat';
%file2 = '/home/fuchs/Desktop/Validation_Results/7/TraindataStatistics/Aimee_DynamixelModel_20091214_1129_standUp_Aimee_DynamixelModel_20091202_1230_0.000241033_9359.val.stat';


valueName = 'Simulation Data - /Sim/ASeries/Right/ShoulderYaw/JointAngle';



figure;
hold on;
xlabel('Time Steps');
ylabel('Measured Data');
dataLength = plotOrcsStatisticsValues(file1, valueName, 1, 'k');

dataLength2 = plotOrcsStatisticsValues(file2, valueName, 1, '--k');

legend('Parameter Set 1','Parameter Set 3',2);


if(dataLength ~= dataLength2)
   error('Different datalength!'); 
end

axis([0, dataLength, 0, 1]);


hold off;
end