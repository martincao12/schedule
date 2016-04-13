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
            output_var.write('*'*300+'\n')
            output_var.write('basic info\n')
            output_var.write('-'*300+'\n')
            output_var.write('%-20s %d\n'%('engine_in_process',config['engine_in_process']))
            output_var.write('%-20s %d\n'%('engine_to_start',config['engine_to_start']))
            output_var.write('%-20s %d\n'%('steps',(config['engine_in_process']+config['engine_to_start'])*10+2))
            output_var.write('%-20s %d\n'%('resources',10+config['facility_num']))
            output_var.write('*'*300+'\n')

            output_var.write('resource availability\n');
            output_var.write('-'*300+'\n')
            for k in range(0,10+config['facility_num']):
                output_var.write('R%-4s\t'%(str(k)))
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
            output_var.write('*'*300+'\n')
            output_var.write('engine info\n')
            output_var.write('-'*300+'\n')
            output_var.write('%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t\n'%('engine_no','engine_type','isDaxiu','hasStarted','due_date'))
            engine_list=[]
            for k in range(0,config['engine_in_process']+config['engine_to_start']):
                engine={}
                engine['engine_no']=k
                engine['engine_type']=random.randint(0,config['engine_type_num']-1)
                engine['isDaxiu']=random.randint(0,1)
                engine['hasStarted']=1 if k<config['engine_in_process'] else 0
                engine['due_date']=config['due_date'] if engine['hasStarted']==1 else (config['due_date'] if random.randint(0,config['engine_to_start']-1)<=config['engine_to_start']-config['engine_in_process'] else config['next_due_date'])
                engine_list.append(engine)
            for engine in engine_list:
                output_var.write('%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t\n'%(str(engine['engine_no']),str(engine['engine_type']),str(engine['isDaxiu']),str(engine['hasStarted']),str(engine['due_date'])));
            output_var.write('*'*300+'\n')

            output_var.write('step info\n')
            output_var.write('-'*300+'\n')
            output_var.write('%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t\n'%('step_no','step_name','engine_no','status','resource_occupation','stand_by_time','due_time','punish_weight','schedule_start_time','modes','successors','successor_list'))
            successor_list_str=""
            for k in range(0,config['engine_in_process']+config['engine_to_start']):
                successor_list_str=successor_list_str+str(k*10+1)+" "
            output_var.write('%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t\n'%('0','dummy_start','-1','0','-1','0','10000','0','0','1',str(config['engine_in_process']+config['engine_to_start']),successor_list_str))
            step_list=[]
            index=0
            step_name_list=['buzhuang','chuanzhuang','zongzhuang','shiche','fenjie','gujian','buzhuang','chuanzhuang','zongzhuang','shiche']
            resource_availability=[config['buzhuang_resource'],config['chuanzhuang_resource'],config['zongzhuang_resource'],config['fenjie_resource'],config['gujian_resource'],config['buzhuang_resource'],config['chuanzhuang_resource'],config['zongzhuang_resource'],config['fenjie_resource'],config['gujian_resource']]
            for k in range(0,config['facility_num']):
                resource_availability.append(1)
            for engine in engine_list:
                jindu=-1
                resource_occupation=-1
                jindu_status=1
                if(engine['hasStarted']==1):
                    count=0
                    while(1==1):
                        count=count+1
                        if(count>1000):
                            jindu=random.randint(0,9)
                            resource_occupation=-1
                            jindu_status=1
                            break
                        jindu=random.randint(0,9)
                        if(engine['isDaxiu']==0):
                            if(jindu==0 or jindu==6):
                                if(resource_availability[0]>0):
                                    resource_availability[0]=resource_availability[0]-1
                                    resource_occupation=0
                                    jindu_status=2
                                    break
                            if(jindu==1 or jindu==7):
                                if(resource_availability[1]>0):
                                    resource_availability[1]=resource_availability[1]-1
                                    resource_occupation=1
                                    jindu_status=2
                                    break
                            if(jindu==2 or jindu==8):
                                if(resource_availability[2]>0):
                                    resource_availability[2]=resource_availability[2]-1
                                    resource_occupation=2
                                    jindu_status=2
                                    break
                            if(jindu==4):
                                if(resource_availability[3]>0):
                                    resource_availability[3]=resource_availability[3]-1
                                    resource_occupation=3
                                    jindu_status=2
                                    break
                            if(jindu==5):
                                if(resource_availability[4]>0):
                                    resource_availability[4]=resource_availability[4]-1
                                    resource_occupation=4
                                    jindu_status=2
                                    break
                            if(jindu==3 or jindu==9):
                                isFeasible=0
                                for k in range(0,engine_type_list[engine['engine_type']]['facility_num']):
                                    if(resource_availability[10+engine_type_list[engine['engine_type']]['facility_list'][k]]>0):
                                        resource_availability[10+engine_type_list[engine['engine_type']]['facility_list'][k]]=resource_availability[10+engine_type_list[engine['engine_type']]['facility_list'][k]]-1
                                        isFeasible=1
                                        resource_occupation=10+engine_type_list[engine['engine_type']]['facility_list'][k]
                                        jindu_status=2
                                        break
                                if(isFeasible==1):
                                    break
                        if(engine['isDaxiu']==1):
                            if(jindu==0 or jindu==6):
                                if(resource_availability[5]>0):
                                    resource_availability[5]=resource_availability[5]-1
                                    resource_occupation=5
                                    jindu_status=2
                                    break
                            if(jindu==1 or jindu==7):
                                if(resource_availability[6]>0):
                                    resource_availability[6]=resource_availability[6]-1
                                    resource_occupation=6
                                    jindu_status=2
                                    break
                            if(jindu==2 or jindu==8):
                                if(resource_availability[7]>0):
                                    resource_availability[7]=resource_availability[7]-1
                                    resource_occupation=7
                                    jindu_status=2
                                    break
                            if(jindu==4):
                                if(resource_availability[8]>0):
                                    resource_availability[8]=resource_availability[8]-1
                                    resource_occupation=8
                                    jindu_status=2
                                    break
                            if(jindu==5):
                                if(resource_availability[9]>0):
                                    resource_availability[9]=resource_availability[9]-1
                                    resource_occupation=9
                                    jindu_status=2
                                    break
                            if(jindu==3 or jindu==9):
                                isFeasible=0
                                for k in range(0,engine_type_list[engine['engine_type']]['facility_num']):
                                    if(resource_availability[10+engine_type_list[engine['engine_type']]['facility_list'][k]]>0):
                                        resource_availability[10+engine_type_list[engine['engine_type']]['facility_list'][k]]=resource_availability[10+engine_type_list[engine['engine_type']]['facility_list'][k]]-1
                                        isFeasible=1
                                        resource_occupation=10+engine_type_list[engine['engine_type']]['facility_list'][k]
                                        jindu_status=2
                                        break
                                if(isFeasible==1):
                                    break

                for k in range(0,10):
                    step_no_str=str(index*10+k+1)
                    step_name_str=step_name_list[k]
                    engine_no_str=engine['engine_no']
                    status_str='0'
                    resource_occupation_str='-1'
                    stand_by_time_str='0'
                    due_time_str='10000'
                    punish_weight_str='0'
                    modes_str='1'
                    successors_str='1'
                    successor_list_str=str(index*10+k+1+1)
                    schedule_start_time_str='0'
                    if(k==9):
                        due_time_str=str(engine['due_date'])
                        punish_weight_str=str(random.uniform(0.5,1.5))[0:4]
                        modes_str=engine_type_list[engine['engine_type']]['facility_num']
                        successor_list_str=str((config['engine_in_process']+config['engine_to_start'])*10+1)
                    if(engine['hasStarted']==0):
                        status_str='0'
                        resource_occupation='-1'
                        if(k==0):
                            stand_by_time_str=str(random.randint(0,5));
                    else:
                        if(k<jindu):
                            status_str='3'
                            resource_occupation_str='-1'
                            schedule_start_time_str='0'
                        if(k==jindu):
                            status_str=str(jindu_status)
                            resource_occupation_str=str(resource_occupation)
                            if(k==0 or k==6):
                                schedule_start_time_str=str(random.randint(-engine_type_list[engine['engine_type']]['buzhuang_time']+1,0))
                            if(k==1 or k==7):
                                schedule_start_time_str=str(random.randint(-engine_type_list[engine['engine_type']]['chuanzhuang_time']+1,0))
                            if(k==2 or k==8):
                                schedule_start_time_str=str(random.randint(-engine_type_list[engine['engine_type']]['zongzhuang_time']+1,0))
                            if(k==3 or k==9):
                                schedule_start_time_str=str(random.randint(-engine_type_list[engine['engine_type']]['shiche_time']+1,0))
                            if(k==4):
                                schedule_start_time_str=str(random.randint(-engine_type_list[engine['engine_type']]['fenjie_time']+1,0))
                            if(k==5):
                                schedule_start_time_str=str(random.randint(-engine_type_list[engine['engine_type']]['gujian_time']+1,0))
                        if(k>jindu):
                            status_str='1'
                            resource_occupation_str='-1'
                            schedule_start_time_str='0'
                    output_var.write('%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t\n'%(step_no_str,step_name_str,engine_no_str,status_str,resource_occupation_str,stand_by_time_str,due_time_str,punish_weight_str,schedule_start_time_str,modes_str,successors_str,successor_list_str))
                index=index+1
            output_var.write('%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t\n'%(str((config['engine_in_process']+config['engine_to_start'])*10+1),'dummy_end','-1','0','-1','0','10000','0','0','1','0',''))
            output_var.write('*'*300+'\n')

            
            output_var.write('step mode info\n')
            output_var.write('-'*300+'\n')
            output_var.write('%-20s\t%-20s\t%-20s\t'%('step_no','mode_no','duration'))
            for k in range(0,10+config['facility_num']):
                output_var.write('R%-4s\t'%(str(k)))
            output_var.write('\n')
            output_var.write('%-20s\t%-20s\t%-20s\t'%('0','0','0'))
            for k in range(0,10+config['facility_num']):
                output_var.write('%-5s\t'%(str(0)))
            output_var.write('\n')

            index=0
            for engine in engine_list:
                for k in range(0,10):
                    if(engine['isDaxiu']==0):
                        if(k==0 or k==6):
                            output_var.write('%-20s\t%-20s\t%-20s\t'%(str(index*10+k+1),'0',str(engine_type_list[engine['engine_type']]['buzhuang_time'])))
                            for l in range(0,10+config['facility_num']):
                                if(l==0):
                                    output_var.write('%-5s\t'%(str(1)))
                                else:
                                    output_var.write('%-5s\t'%(str(0)))
                            output_var.write('\n')
                        if(k==1 or k==7):
                            output_var.write('%-20s\t%-20s\t%-20s\t'%(str(index*10+k+1),'0',str(engine_type_list[engine['engine_type']]['chuanzhuang_time'])))
                            for l in range(0,10+config['facility_num']):
                                if(l==1):
                                    output_var.write('%-5s\t'%(str(1)))
                                else:
                                    output_var.write('%-5s\t'%(str(0)))
                            output_var.write('\n')
                        if(k==2 or k==8):
                            output_var.write('%-20s\t%-20s\t%-20s\t'%(str(index*10+k+1),'0',str(engine_type_list[engine['engine_type']]['zongzhuang_time'])))
                            for l in range(0,10+config['facility_num']):
                                if(l==2):
                                    output_var.write('%-5s\t'%(str(1)))
                                else:
                                    output_var.write('%-5s\t'%(str(0)))
                            output_var.write('\n')
                        if(k==4):
                            output_var.write('%-20s\t%-20s\t%-20s\t'%(str(index*10+k+1),'0',str(engine_type_list[engine['engine_type']]['fenjie_time'])))
                            for l in range(0,10+config['facility_num']):
                                if(l==3):
                                    output_var.write('%-5s\t'%(str(1)))
                                else:
                                    output_var.write('%-5s\t'%(str(0)))
                            output_var.write('\n')
                        if(k==5):
                            output_var.write('%-20s\t%-20s\t%-20s\t'%(str(index*10+k+1),'0',str(engine_type_list[engine['engine_type']]['gujian_time'])))
                            for l in range(0,10+config['facility_num']):
                                if(l==4):
                                    output_var.write('%-5s\t'%(str(1)))
                                else:
                                    output_var.write('%-5s\t'%(str(0)))
                            output_var.write('\n')
                        if(k==3 or k==9):
                            for t in range(0,engine_type_list[engine['engine_type']]['facility_num']):
                                output_var.write('%-20s\t%-20s\t%-20s\t'%(str(index*10+k+1),str(t),str(engine_type_list[engine['engine_type']]['shiche_time'])))
                                for l in range(0,10+config['facility_num']):
                                    if(l==10+engine_type_list[engine['engine_type']]['facility_list'][t]):
                                        output_var.write('%-5s\t'%(str(1)))
                                    else:
                                        output_var.write('%-5s\t'%(str(0)))
                                output_var.write('\n')
                    else:
                        if(k==0 or k==6):
                            output_var.write('%-20s\t%-20s\t%-20s\t'%(str(index*10+k+1),'0',str(engine_type_list[engine['engine_type']]['buzhuang_time'])))
                            for l in range(0,10+config['facility_num']):
                                if(l==5):
                                    output_var.write('%-5s\t'%(str(1)))
                                else:
                                    output_var.write('%-5s\t'%(str(0)))
                            output_var.write('\n')
                        if(k==1 or k==7):
                            output_var.write('%-20s\t%-20s\t%-20s\t'%(str(index*10+k+1),'0',str(engine_type_list[engine['engine_type']]['chuanzhuang_time'])))
                            for l in range(0,10+config['facility_num']):
                                if(l==6):
                                    output_var.write('%-5s\t'%(str(1)))
                                else:
                                    output_var.write('%-5s\t'%(str(0)))
                            output_var.write('\n')
                        if(k==2 or k==8):
                            output_var.write('%-20s\t%-20s\t%-20s\t'%(str(index*10+k+1),'0',str(engine_type_list[engine['engine_type']]['zongzhuang_time'])))
                            for l in range(0,10+config['facility_num']):
                                if(l==7):
                                    output_var.write('%-5s\t'%(str(1)))
                                else:
                                    output_var.write('%-5s\t'%(str(0)))
                            output_var.write('\n')
                        if(k==4):
                            output_var.write('%-20s\t%-20s\t%-20s\t'%(str(index*10+k+1),'0',str(engine_type_list[engine['engine_type']]['fenjie_time'])))
                            for l in range(0,10+config['facility_num']):
                                if(l==8):
                                    output_var.write('%-5s\t'%(str(1)))
                                else:
                                    output_var.write('%-5s\t'%(str(0)))
                            output_var.write('\n')
                        if(k==5):
                            output_var.write('%-20s\t%-20s\t%-20s\t'%(str(index*10+k+1),'0',str(engine_type_list[engine['engine_type']]['gujian_time'])))
                            for l in range(0,10+config['facility_num']):
                                if(l==9):
                                    output_var.write('%-5s\t'%(str(1)))
                                else:
                                    output_var.write('%-5s\t'%(str(0)))
                            output_var.write('\n')
                        if(k==3 or k==9):
                            for t in range(0,engine_type_list[engine['engine_type']]['facility_num']):
                                output_var.write('%-20s\t%-20s\t%-20s\t'%(str(index*10+k+1),str(t),str(engine_type_list[engine['engine_type']]['shiche_time'])))
                                for l in range(0,10+config['facility_num']):
                                    if(l==10+engine_type_list[engine['engine_type']]['facility_list'][t]):
                                        output_var.write('%-5s\t'%(str(1)))
                                    else:
                                        output_var.write('%-5s\t'%(str(0)))
                                output_var.write('\n')
                index=index+1
            output_var.write('%-20s\t%-20s\t%-20s\t'%(str((config['engine_in_process']+config['engine_to_start'])*10+1),'0','0'))
            for k in range(0,10+config['facility_num']):
                output_var.write('%-5s\t'%(str(0)))
            output_var.write('\n')

            output_var.write('*'*300+'\n')
            output_var.close()
readConfig()
dataGen()