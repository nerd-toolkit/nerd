function findCertainDiffData(data, group1, file1, group2 , file2)


for i = 1:length(data)
   
    if( strcmp(data{i,1}.group,group1) && ...
        strcmp(data{i,1}.paraset,file1) && ...
        strcmp(data{i,2}.group,group2) && ...
        strcmp(data{i,2}.paraset,file2) )
    
        fprintf('%d\n',data{i,3})
    
    end
   
    
    
    if( strcmp(data{i,1}.group,group2) && ...
        strcmp(data{i,1}.paraset,file2) && ...
        strcmp(data{i,2}.group,group1) && ...
        strcmp(data{i,2}.paraset,file1) )
    
        fprintf('%d\n',data{i,3})
    
    end    
end

end