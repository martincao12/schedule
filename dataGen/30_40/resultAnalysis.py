import numpy
config={}
algorithm_list=['RAND','LFT','HGA','HSS','JPSO']
def readConfig():
    infile=open('config.txt','r')
    lines=infile.readlines()
    for line in lines[0:len(lines)]:
        config[line.split(":")[0]]=int(line.split(":")[1])

def analyze():
    readConfig()
    outfile_name="result_compare.txt"
    outfile=open(outfile_name,'w')
    outfile.write('*'*300+'\n')
    outfile.write('basic info\n')
    outfile.write('-'*300+'\n')
    outfile.write('%-20s %d\n'%('engine_in_process',config['engine_in_process']))
    outfile.write('%-20s %d\n'%('engine_to_start',config['engine_to_start']))
    outfile.write('%-20s %d\n'%('steps',(config['engine_in_process']+config['engine_to_start'])*10+2))
    outfile.write('%-20s %d\n'%('resources',10+config['facility_num']))
    outfile.write('*'*300+'\n')

    outfile.write('resource availability\n');
    outfile.write('-'*300+'\n')
    for k in range(0,10+config['facility_num']):
        outfile.write('R%-4s\t'%(str(k)))
    outfile.write("\n")
    outfile.write('%-5s\t'%(str(config['buzhuang_resource'])))
    outfile.write('%-5s\t'%(str(config['chuanzhuang_resource'])))
    outfile.write('%-5s\t'%(str(config['zongzhuang_resource'])))
    outfile.write('%-5s\t'%(str(config['fenjie_resource'])))
    outfile.write('%-5s\t'%(str(config['gujian_resource'])))
    outfile.write('%-5s\t'%(str(config['buzhuang_resource'])))
    outfile.write('%-5s\t'%(str(config['chuanzhuang_resource'])))
    outfile.write('%-5s\t'%(str(config['zongzhuang_resource'])))
    outfile.write('%-5s\t'%(str(config['fenjie_resource'])))
    outfile.write('%-5s\t'%(str(config['gujian_resource'])))
    for k in range(10,10+config['facility_num']):
        outfile.write('%-5s\t'%(str(1)))
    outfile.write("\n")
    outfile.write('*'*300+'\n')
    outfile.write('algorithm compare\n');
    outfile.write('-'*300+'\n')
    outfile.write('%-10s\t'%('datafile'))
    outfile.write(('%-'+str(len(algorithm_list)*20)+'s\t|')%('time_cost'))
    outfile.write(('%-'+str(len(algorithm_list)*20)+'s\t|')%('ontime_proportion'))
    outfile.write(('%-'+str(len(algorithm_list)*20)+'s\t|')%('max_delay'))
    outfile.write(('%-'+str(len(algorithm_list)*20)+'s\t|')%('avg_delay'))
    outfile.write(('%-'+str(len(algorithm_list)*20)+'s\t|')%('avg_flowtime'))
    outfile.write(('%-'+str(len(algorithm_list)*20)+'s\t|\n')%('object_function_value'))
    time_cost=[]
    ontime_proportion=[]
    max_delay=[]
    avg_delay=[]
    avg_flowtime=[]
    object_function_value=[]
    for i in range(0,len(algorithm_list)):
        time_cost.append([])
        ontime_proportion.append([])
        max_delay.append([])
        avg_delay.append([])
        avg_flowtime.append([])
        object_function_value.append([])
    outfile.write('%-10s\t'%(''))
    for line in range(0,6):
        for k in range(0,len(algorithm_list)-1):
            outfile.write('%-18s\t'%(algorithm_list[k]))
        outfile.write('%-20s\t|'%(algorithm_list[len(algorithm_list)-1]))
    outfile.write('\n')
    for i in range(0,config['data_base_num']):
        for j in range(0,config['data_var_num']):
            infile_list=[]
            for algorithm in algorithm_list:
                infile=open('dataInput_'+str(i)+'_'+str(j)+'_'+algorithm+'result.txt','r')
                lines=infile.readlines()
                infile.close()
                infile_list.append(lines)
            outfile.write('%-10s\t'%(str(i)+'_'+str(j)))
            for line in range(0,6):
                for k in range(0,len(infile_list)-1):
                    outfile.write('%-18s\t'%(infile_list[k][line].split(':')[1].split('\n')[0]))
                outfile.write('%-20s\t|'%(infile_list[len(infile_list)-1][line].split(':')[1].split('\n')[0]))
            for k in range(0,len(algorithm_list)):
                time_cost[k].append(float(infile_list[k][0].split(':')[1].split('ms\n')[0]))
                ontime_proportion[k].append(float(infile_list[k][1].split(':')[1].split('\n')[0]))
                max_delay[k].append(float(infile_list[k][2].split(':')[1].split('\n')[0]))
                avg_delay[k].append(float(infile_list[k][3].split(':')[1].split('\n')[0]))
                avg_flowtime[k].append(float(infile_list[k][4].split(':')[1].split('\n')[0]))
                object_function_value[k].append(float(infile_list[k][5].split(':')[1].split('\n')[0]))
            outfile.write('\n')
    performance=[]
    performance.append(time_cost)
    performance.append(ontime_proportion)
    performance.append(max_delay)
    performance.append(avg_delay)
    performance.append(avg_flowtime)
    performance.append(object_function_value)
    performace=numpy.array(performance)
    
    
    outfile.write('-'*300+'\n')
    outfile.write('%-10s\t'%('avg'))
    for line in range(0,6):
        for k in range(0,len(algorithm_list)-1):
            outfile.write('%-18s\t'%(numpy.average(performace[line][k])))
        outfile.write('%-20s\t|'%(numpy.average(performace[line][len(algorithm_list)-1])))
    outfile.write('\n')
    outfile.write('%-10s\t'%('max'))
    for line in range(0,6):
        for k in range(0,len(algorithm_list)-1):
            outfile.write('%-18s\t'%(numpy.max(performace[line][k])))
        outfile.write('%-20s\t|'%(numpy.max(performace[line][len(algorithm_list)-1])))
    outfile.write('\n')
    outfile.write('%-10s\t'%('min'))
    for line in range(0,6):
        for k in range(0,len(algorithm_list)-1):
            outfile.write('%-18s\t'%(numpy.min(performace[line][k])))
        outfile.write('%-20s\t|'%(numpy.min(performace[line][len(algorithm_list)-1])))
    outfile.write('\n')
    outfile.write('*'*300+'\n')

analyze()
