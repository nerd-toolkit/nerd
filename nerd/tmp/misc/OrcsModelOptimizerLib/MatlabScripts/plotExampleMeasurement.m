function plotExampleMeasurement()

file = '/media/VERBATIM/Documents/Bachelorarbeit/Experimental Data/Optimization_Runs_Results/Powell/4/Aimee_DynamixelModel_20091207_1646/TraindataStatistics/Aimee_DynamixelModel_20091207_1646_shoulderYaw_AfterOptimization.stat';
pValue = 'Physical Data - /Sim/ASeries/Right/ShoulderYaw/JointAngle';
sValue = 'Simulation Data - /Sim/ASeries/Right/ShoulderYaw/JointAngle';

figure;
hold on;
xlabel('Time Steps');
ylabel('Measured Data');
dataLength = plotOrcsStatisticsValues(file, pValue, 1, 'b');

dataLength2 = plotOrcsStatisticsValues(file, sValue, 1, 'g');

legend('Physical Data','Simulation Data',2);


if(dataLength ~= dataLength2)
   error('Different datalength!'); 
end

axis([0, dataLength, 0, 1]);


hold off;
end