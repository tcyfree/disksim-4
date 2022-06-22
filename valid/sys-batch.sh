rm out-batch.txt
for i in $(seq 1 100)
do   
    ../src/syssim $i 0 0 >> out-batch.txt
done

for i in $(seq 1 100)
do   
    ../src/syssim $i 0 1 >> out-batch.txt
done 

 