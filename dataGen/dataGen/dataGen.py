import random
config={}

def readConfig():
    infile=open('config.txt','r')
    lines=infile.readlines()
    for line in lines[0:len(lines)]:
        config[line.split(":")[0]]=int(line.split(":")[1])

def dataGen():
    for i in range(0,config['data_base_num']):
        outfile_base_filename="engine_type_info_"+str(i)+".txt"
        outfile_base=open(outfile_base_filename,'w')
        outfile_base.write('%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t\n'%('type_id','buzhuang_time','chuanzhuang_time','zongzhuang_time','fenjie_time','gujian_time','shiche_time','facility_num','facility_list'))
        engine_type_list=[]
        for j in range(0,config['engine_type_num']):
            engine_type={}
            engine_type['type_id']=j
            engine_type['buzhuang_time']=random.randint(config['buzhuang_time_lb'],config['buzhuang_time_ub'])
            engine_type['chuanzhuang_time']=random.randint(config['chuanzhuang_time_lb'],config['chuanzhuang_time_ub'])
            engine_type['zongzhuang_time']=random.randint(config['zongzhuang_time_lb'],config['zongzhuang_time_ub'])
            engine_type['fenjie_time']=random.randint(config['fenjie_time_lb'],config['fenjie_time_ub'])
            engine_type['gujian_time']=random.randint(config['gujian_time_lb'],config['gujian_time_ub'])
            engine_type['shiche_time']=random.randint(config['shiche_time_lb'],config['shiche_time_ub'])
            engine_type['facility_num']=random.randint(1,config['facility_num'])
            engine_type['facility_list']=[]
            while len(engine_type['facility_list'])<engine_type['facility_num']:
                random_facility=random.randint(0,config['facility_num']-1)
                if random_facility not in engine_type['facility_list']:
                    engine_type['facility_list'].append(random_facility);
            engine_type['facility_list'].sort()
            engine_type_list.append(engine_type);
        for engine_type in engine_type_list:
            str_tem=""
            for facility in engine_type['facility_list']:
                str_tem=str_tem+str(facility)+'\t'
            outfile_base.write('%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t\n'%(engine_type['type_id'],engine_type['buzhuang_time'],engine_type['chuanzhuang_time'],engine_type['zongzhuang_time'],engine_type['fenjie_time'],engine_type['gujian_time'],engine_type['shiche_time'],engine_type['facility_num'],str_tem));
        outfile_base.close()
        
        for j in range(0,config['data_var_num']):
            output_var_filename="dataInput_"+str(i)+"_"+str(j)+".txt"
            output_var=open(output_var_filename,'w')
            output_var.write('*'*200+'\n')
            output_var.write('basic info\n')
            output_var.write('-'*200+'\n')
            output_var.write('%-20s %d\n'%('engine_in_process',config['engine_in_process']))
            output_var.write('%-20s %d\n'%('engine_to_start',config['engine_to_start']))
            output_var.write('%-20s %d\n'%('steps',(config['engine_in_process']+config['engine_to_start'])*10+2))
            output_var.write('%-20s %d\n'%('resources',10+config['facility_num']))
            output_var.write('*'*200+'\n')

            output_var.write('resource availability\n');
            output_var.write('-'*200+'\n')
            for k in range(0,10+config['facility_num']):
                output_var.write('R%-5s\t'%(str(k)))
            output_var.write("\n")
            output_var.write('%-5s\t'%(str(config['buzhuang_resource'])))
            output_var.write('%-5s\t'%(str(config['chuanzhuang_resource'])))
            output_var.write('%-5s\t'%(str(config['zongzhuang_resource'])))
            output_var.write('%-5s\t'%(str(config['fenjie_resource'])))
            output_var.write('%-5s\t'%(str(config['gujian_resource'])))
            output_var.write('%-5s\t'%(str(config['buzhuang_resource'])))
            output_var.write('%-5s\t'%(str(config['chuanzhuang_resource'])))
            output_var.write('%-5s\t'%(str(config['zongzhuang_resource'])))
            output_var.write('%-5s\t'%(str(config['fenjie_resource'])))
            output_var.write('%-5s\t'%(str(config['gujian_resource'])))
            for k in range(10,10+config['facility_num']):
                output_var.write('%-5s\t'%(str(1)))
            output_var.write("\n")
            output_var.write('*'*200+'\n')

            output_var.write('engine info\n')
            output_var.write('-'*200+'\n')
            output_var.write('%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t\n'%('engine_no','engine_type','isDaxiu','hasStarted','due_date'))
            engine_list=[]
            for k  in range(0,config['engine_in_process']+config['engine_to_start']):
                engine={}
                engine['engine_no']=k
                engine['engine_type']=random.randint(0,config['engine_type_num']-1)
                engine['isDaxiu']=random.randint(0,1)
                engine['hasStarted']=1 if k<config['engine_in_process'] else 0
                engine['due_date']=config['due_date'] if engine['hasStarted']==1 else (config['due_date'] if random.randint(0,config['engine_to_start']-1)<=config['engine_to_start']-config['engine_in_process'] else config['next_due_date'])
                engine_list.append(engine)
            for engine in engine_list:
                output_var.write('%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t\n'%(str(engine['engine_no']),str(engine['engine_type']),str(engine['isDaxiu']),str(engine['hasStarted']),str(engine['due_date'])));
            output_var.write('*'*200+'\n')

            output_var.write('step info\n')
            output_var.write('-'*200+'\n')
            output_var.write('')
            step_list=[]
            

            output_var.close()
readConfig()
dataGen()