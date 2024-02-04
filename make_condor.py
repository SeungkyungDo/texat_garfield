import os

submit_all = open("ss_all.sh","w");

#ne = 1
#nx = 2
#dx = 1
#xMin = 0.
#xMax = 20.
ne = 100
nx = 100
dx = 0.01
xMin = 0.
xMax = 20.
name = "submit_tgs"
input_path_list = ""
executable_name = "texat_gas_simulation"

init_dir = f"{os.getcwd()}/condor"
os.makedirs(init_dir, exist_ok=True)

for i in range(0,nx):
    submit_file_name = f"{init_dir}/{name}_{i}.sh"
    submit_file = open(submit_file_name,"w");
    x1 = xMin+xMax/nx*i
    x2 = xMin+xMax/nx*(i+1)

    content = f"""#
Executable     = {executable_name}
Arguments      = {ne} {dx} {x1} {x2} -b -q
Universe       = vanilla
GetEnv         = True
Request_cpus   = 1
Request_memory = 300
Log            = {name}_{i}.log
Error          = {name}_{i}.err
Output         = {name}_{i}.out

Initialdir      = {init_dir}
#Notification   = Always
#Notify_user    = YOUR_EMAIL
#Input          = {input_path_list}
#transfer_input_files = {input_path_list}

should_transfer_files = YES

Queue
"""

    print(content,file=submit_file)
    print(f"echo condor_submit {submit_file_name}",file=submit_all)
    print(f"condor_submit {submit_file_name}",file=submit_all)
