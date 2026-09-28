//
//
//  assg
//
//  Created by James Fu on 2026/9/26.
//

#include <stdio.h>
//Recive argument from the terminal 接收终端后面跟着的参数。
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Invalid number of arguments\n"); //if there is incorrect listfile then it stop 如果用户没有正确给我一个list_file，那就报错并停止。
        return 1;}
    FILE *fp; //create a file pointer
    fp = fopen(argv[1], "r"); //open the file argv[1] with the file pointer
    
    if (fp == NULL) { //check if fopen sucess 检查 fopen 到底有没有开成功。例如没有文件就失败
        printf("Error opening file\n");
        return 1; //kill the program if there is a error 结束整个主程序，并告诉系统：程序出问题了。
    }
    
    char file_name[100];//create a char arraywith 100 size to store strings from the file.name file 创建一个 一百char的字 组名字叫 file_name，用来存字符串
    
   
    
    int total= 0;//create total to stroe how many schedule file in the list file 创建，目的是统计 list file 里面一共有多少个 schedule 文件。
    
    while(fscanf(fp, "%s", file_name)==1) { // read lines from the fp pointed file and store it into file_name. Stop when there is the end of file 从 fp 指向的文件里读取一个字符串，存进 file_name。读不到东西了就停止
        total++;
    }
    printf("Processing %d schedule files...\n", total);//print a message after loop end
    
    fclose(fp); //close the file 当第一个循环结束的时候，文件虽然还开着，但是指针已经在文件最末尾了。第二个扫描 //一上来就读不到东西，第二个循环一次都不会执行。
    
    fp = fopen(argv[1], "r"); //open file again 重开
    
    int count = 0;// create count, shows which schedule file are we processing 创建一个整数 表示当前处理到第几个 schedule file 了。
    
    while (fscanf(fp, "%s", file_name) == 1) { //read from the list_file and store the string into fillenumber 从指针指向的listf里面读取一个字符串存到filen
        
        count++; //everytime it read one, count will add 1 to itself 每次读到一个 count就加一
        
        FILE *schedule_fp; //use the new filename and open it. so create a new pointer 拿着从 list_file.txt 读出来的文件名，把那个 schedule 文件真正打开。
        //读到哪个就开哪个文件
        schedule_fp = fopen(file_name, "r"); //要处理两层文件所以创建新指针，去某一个 schedule file
        
        //check if the file exist 如果文件不存在、路径不对等，fopen 失败 就停止
        if (schedule_fp == NULL) {
            printf("Error opening schedule file\n");
            return 1;}
        
        char route_id[20]; //store route id存路线编号（为什么需要整数是因为有的带字母）
        char direction = '\0'; //store direction存储方向
        int number_stops; //store number of stops 存站数，
        
        fscanf(schedule_fp, "%s", route_id);// read the route id, which is the first part
        
        char ignored_name[100]; //create a char array to store the name we dont need 它的目的就是临时接住我们不需要的站名
        
        fscanf(schedule_fp, "%s", ignored_name);//read the sec part which is the stop name we dont need
        
        fscanf(schedule_fp, "%d", &number_stops); //read the third part whcih is hwo many stops of a route, store that in num_stops, 需要&因为 %d 要把读到的整数写进 num_stops 的内存地址。
        
        //Find the direction, check each string until it ends 找方向，从0开始只要还没走到字符串结尾，就继续，每次往后检查一个字符。
        
        
        for (int i = 0; file_name[i] != '\0'; i++) { //只要当前字符还不是字符串结尾，就继续。
           //Check if the file name contains X. If it does, add X to the end of route_id.
            //检查文件名里面有没有 X。如果有，就把 X 加到 route_id 的最后面。
            if (file_name[i] == 'X') {
                int j = 0; //Start from the first character of route_id 从 route_id 第一个字符开始

                //Find the end of route_id by looking for '\0'
                    //一直往后找，直到找到 route_id 字符串结尾的 '\0'
                while (route_id[j] != '\0') {
                    j++;
                }
                //Replace the old '\0' with X
                //把原来字符串结尾的 '\0' 换成 X
                route_id[j] = 'X';
                
                //Add a new '\0' after X to make sure the string ends correctly
                //在 X 后面放一个新的 '\0'，保证字符串正确结束
                
                route_id[j + 1] = '\0';
            }
            
            //Find the direction with the format 根据对的格式去找ewns 先找下划线 然后后面的一位
            if (file_name[i] == '_' && // if the current string is _, check if the next one is either EWNS 检查当前字符必须是 _，并且下一个字符必须是 E/W/N/S。
                (file_name[i + 1] == 'E' ||
                 file_name[i + 1] == 'W' ||
                 file_name[i + 1] == 'N' ||
                 file_name[i + 1] == 'S')) {
                
                direction = file_name[i + 1];}} //Direction Found
        
        printf("schedule #%d is route %s it has %d stops and is in the %c direction\n",
               count, route_id, number_stops, direction); // Print all information
        fclose(schedule_fp);// Close the current schedule file
    }
    fclose(fp);
    //Close the list file after all schedules are processed
    
    return 0;} // Program finished successfully
