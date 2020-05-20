%PlotVideo

%This m-script evaluates the Patch_*.csv files and displays them as a
%video.

%%Read in
%load('bin\64\SimulationsErgebnisse\Patch_Temperatures.mat')
% if(exist('Patch_Areas.csv','file') && exist('Patch_Vertices.csv','file') && exist('Patch_Temperatures.csv','file'))    
%     disp('new Simulation results were found')
%     movefile('Patch_*','SimulationsErgebnisse\.')
% elseif(exist('SimulationsErgebnisse\Patch_Areas.csv','file') && exist('SimulationsErgebnisse\Patch_Vertices.csv','file') && exist('SimulationsErgebnisse\Patch_Temperatures.csv','file'))    
%     disp('no new simulation results were found, using existing ones');   
% else
%     error('no simulation results were found')
% end
 disp('Reading in csv-files ...');
 areas = csvread('..\Patch_Areas.csv');
 vertices = csvread('..\Patch_Vertices.csv');
%f = fopen('SimulationsErgebnisse\Patch_Temperatures.csv');
%x = fread(f,1);
% if x == '#'
%     tempVec = csvread('SimulationsErgebnisse\Patch_Temperatures.csv',2,0 );
% else
%     tempVec = csvread('SimulationsErgebnisse\Patch_Temperatures.csv' );
% end

tempVec=diga.daten.Temperature;



areas(areas==0) = NaN;

timeVector = diga.daten.Programmdauer;
simDuration = timeVector(end) - timeVector(1);
speedfactor = input(['Total Simulation time was ',num2str(simDuration),'s.\n Choose a speedfactor > 1 to speed up graphics (simulation_time/speedfactor) (default 10)?: ']);

if(isempty(speedfactor))
    speedfactor = 10;
end

dtVector = diff(timeVector)/speedfactor;

%%Setting figure configuration
figure('units','normalized','outerposition',[0 0 1 1]);
set(gcf,'Renderer','zbuffer');
view([27 55]);
xlabel('x [m]','fontsize',14);
ylabel('y [m]','fontsize',14);
zlabel('z [m]','fontsize',14);
cb = colorbar('vert');
ylabel(cb,'Temperatur [\circ C]','fontsize',14);
caxis([min(min(tempVec))-2,max(max(tempVec))+2]);

    time = num2str(timeVector(1));
    %th = title(time);
    set(gca,'DataAspectRatio',[1 1 1]);
    temperatures =  tempVec(:, 1);
    h = patch('Vertices',vertices,'Faces',areas,'FaceColor','flat','FaceVertexCData',temperatures,'Marker','.','MarkerFaceColor','k');
  fignmr=1;  
for i = 2:50:length(timeVector)
   pause(dtVector(i-1)-0.05);
   %tic
   temperatures =  tempVec(:, i);
   set(h,'FaceVertexCData',temperatures);
   caxis([min(temperatures)-0.25,max(temperatures)+0.25]);
   saveas(gcf,strcat("img",num2str(fignmr),".jpg"))   
   %title(num2str(timeVector(i)));
   fignmr=fignmr+1;
   %toc
end

figure
hold on
for i=1:100:length(temperatures)
    plot(Programmdauer./60,tempVec(i,:))
end
xlabel("Zeit [min]")
ylabel("Temperatur [°C]")