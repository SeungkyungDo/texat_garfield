import os

submit_all = open("ss_all.sh","w");

num_electrons = 100
num_bins = 1000
xMin = -10.
xMax = 10.
name = "submit_tgs"
input_path_list = ""
executable_name = "texat_gas_simulation.exe"

init_dir = f"{os.getcwd()}/condor"
os.makedirs(init_dir, exist_ok=True)

for i in range(0,num_bins):
    submit_file_name = f"{init_dir}/{name}_{i}.sh"
    submit_file = open(submit_file_name,"w");

    content = f"""#
Executable     = {executable_name}
Arguments      = {i} {num_electrons} {num_bins} {xMin} {xMax} 0
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
