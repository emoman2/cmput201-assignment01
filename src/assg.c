
#include <stdio.h>
//Define the maximum sizes of arrays used in the program.
#define MAX_ROUTES 250 // Maximum number of bus routes the program can store.
#define MAX_STOPS 200  // Maximum number of stops in each route.
#define MAX_TAPS 10000 // Maximum number of tap records the program can process.

// Use this function to Convert a date into a total number of days. This function also considers leap years to calculate the correct day number.
long long day_number(int year, int month, int day)
{
    int month_days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    long long y = year - 1;
    long long result = 365 * y + y / 4 - y / 100 + y / 400;
    for (int m = 1; m < month; m++) {
        result += month_days[m - 1];
        if (m == 2 && (year % 400 == 0 ||
                      (year % 4 == 0 && year % 100 != 0))) result++;
    }
    return result + day - 1;
}

//Read one line from the file and remove the newline characters. Return 1 if successful, or 0 if reading fails.
int read_line(FILE *fp, char line[], int capacity)
{
    if (fgets(line, capacity, fp) == NULL) return 0;
    int length = 0;
    while (line[length] != '\0') length++;
    if (length > 0 && line[length - 1] == '\n') line[--length] = '\0';
    else if (!feof(fp)) return 0;
    if (length > 0 && line[length - 1] == '\r') line[--length] = '\0';
    return 1;
}

// This function read a number from a string, skipping spaces and limiting the number of digits. Store the number and update the reading position. Return 1 if successful, or 0 if no digits are found.

int read_number(const char text[], int *position, int max_digits, int *value)
{
    int p = *position;
    while (text[p] == ' ' || text[p] == '\t' ||
           text[p] == '\r' || text[p] == '\n') p++;

    int digits = 0;
    int number = 0;
    while (text[p] >= '0' && text[p] <= '9' && digits < max_digits) {
        number = number * 10 + (text[p] - '0');
        p++;
        digits++;
    }
    if (digits == 0) return 0;
    *position = p;
    *value = number;
    return 1;
}

// Check if the rest of the string is just spaces or empty characters.
// Return 1 if there is nothing else, otherwise return 0. 检查字符串剩下的部分是不是只有空格、换行符这些，没有其他内容就返回 1，否则返回 0。
int only_space(const char text[], int position)
{
    while (text[position] != '\0') {
        char c = text[position];
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n') return 0;
        position++;
    }
    return 1;
}

// Read the text before the next comma and save it into an array.
// Return 0 if the field is empty or too long, otherwise return 1.读取逗号前面的内容，存进 field 数组里。如果内容为空、太长或者没有找到逗号，就返回 0。成功则返回 1，并把读取位置移到逗号后面。
int read_field(const char text[], int *position, char field[], int capacity)
{
    int p = *position;
    int n = 0;
    while (text[p] != ',' && text[p] != '\0') {
        if (n >= capacity - 1) return 0;
        field[n] = text[p];
        n++;
        p++;
    }
    if (n == 0 || text[p] != ',') return 0;
    field[n] = '\0';
    *position = p + 1;
    return 1;
}

// Read the date from a string and separate it into month, day and year.
// Return 0 if the date format is wrong, otherwise return 1.
int read_date(const char text[], int *month, int *day, int *year)
{
    int p = 0;
    if (!read_number(text, &p, 2, month) || text[p] != '-') return 0;
    p++;
    if (!read_number(text, &p, 2, day) || text[p] != '-') return 0;
    p++;
    if (!read_number(text, &p, 4, year)) return 0;
    return only_space(text, p);
}

// Read the time from a string and get the hour, minute and am or pm.
// Return 0 if the format is wrong, otherwise return 1. 从字符串中读取时间，比如 09:30 AM，分别提取小时、分钟和 AM/PM。如果格式不对返回 0，正确返回 1。

int read_clock(const char text[], int *hour, int *minute, char am_pm[])
{
    int p = 0;
    if (!read_number(text, &p, 2, hour) || text[p] != ':') return 0;
    p++;
    if (!read_number(text, &p, 2, minute)) return 0;
    while (text[p] == ' ' || text[p] == '\t' ||
           text[p] == '\r' || text[p] == '\n') p++;
    if (text[p] != 'A' && text[p] != 'P') return 0;
    am_pm[0] = text[p];
    p++;
    if (text[p] != 'M') return 0;
    am_pm[1] = text[p];
    am_pm[2] = '\0';
    p++;
    return only_space(text, p);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Invalid number of arguments\n"); //if there is incorrect listfile then it stop 如果用户没有正确给我一个list_file，那就报错并停止。
        return 1;}
    FILE *fp; // we create a file pointer
    fp = fopen(argv[1], "r"); //open the file argv[1] with the file pointer
    if (fp == NULL) {
        printf("Error reopening file\n");
        return 1;
    }
    char file_name[100];//create a char arraywith 100 size to store strings from the file.name file 创建一个 一百char的字 组名字叫 file_name，用来存字符串
    int stops[MAX_ROUTES][MAX_STOPS];
    char stop_names[MAX_ROUTES][MAX_STOPS][51]; // we need to have at most 50 characters.
    int stop_counts[MAX_ROUTES];
    char route_ids[MAX_ROUTES][20];
    char directions[MAX_ROUTES];
    int matching_routes[MAX_ROUTES];
    char tap_dates[MAX_TAPS][20];
    char tap_times[MAX_TAPS][20];
    char tap_types[MAX_TAPS];
    int tap_stops[MAX_TAPS];
    int tap_used[MAX_TAPS] = {0};
    int known_entry_stops[MAX_TAPS];
    int known_exit_stops[MAX_TAPS];
    int known_hours[MAX_TAPS];
    int known_frequency[MAX_TAPS];
    int known_count = 0;
    int tap_count = 0;
    
    // Read all schedule files below.
    int count = 0;// create count, shows which schedule file are we processing 创建一个整数 表示当前处理到第几个 schedule file 了。
    while (fscanf(fp, "%99s", file_name) == 1) { //read from the list_file and store the string into fillenumber 从指针指向的listf里面读取一个字符串存到filen
        count++; //everytime it read one, count will add 1 to itself 每次读到一个 count就加一
        if (count > MAX_ROUTES) { printf("Schedule exceeds array capacity\n"); fclose(fp); return 1; }
        FILE *schedule_fp; //use the new filename and open it. so create a new pointer 拿着从 list_file.txt 读出来的文件名，把那个 schedule 文件真正打开。
        //读到哪个就开哪个文件
        schedule_fp = fopen(file_name, "r"); //要处理两层文件所以创建新指针，去某一个 schedule file
        //check if the file exist 如果文件不存在、路径不对等，fopen 失败 就停止
        if (schedule_fp == NULL) {
            printf("Error opening schedule file\n");  // Check if the schedule file opened successfully. // Then create variables to store the route ID, direction and number of stops.
            return 1;}
        char route_id[20];
        char direction = '\0';
        int number_stops = 0;
        
        // Read the route ID and the next word from the schedule file.
        // If reading fails, show an error and close the files.
        if (fscanf(schedule_fp, "%19s", route_id) != 1) {
            printf("Invalid schedule data\n"); fclose(schedule_fp); fclose(fp); return 1;
        }
        char ignored_name[100];
        if (fscanf(schedule_fp, "%99s", ignored_name) != 1) {
            printf("Invalid schedule data\n"); fclose(schedule_fp); fclose(fp); return 1;
        }
        if (fscanf(schedule_fp, "%d", &number_stops) != 1) {
            printf("Error reading number of stops\n");
            fclose(schedule_fp);
            fclose(fp);
            return 1;
        }
        //Read the next word from the schedule file.
        // Check if the number of routes and stops is within the array limits.
        // If something is wrong, show an error and stop the program.
        char ignored_word[20];
        if (fscanf(schedule_fp, "%19s", ignored_word) != 1) {
            printf("Invalid schedule data\n"); fclose(schedule_fp); fclose(fp); return 1;
        }
        if (count > MAX_ROUTES ||
            number_stops < 0 ||
            number_stops > MAX_STOPS) {
            printf("Schedule exceeds array capacity\n");
            fclose(schedule_fp);
            fclose(fp);
            return 1;
        }
        // Save the number of stops and read each stop's ID and name.
        // Store them in the arrays and check if the data is valid.
        // If the stop name is too long or reading fails, stop the program.
        
        stop_counts[count - 1] = number_stops;
        int stop_id;
        char stop_name[100];
        for (int i = 0; i < number_stops; i++) {
            if (fscanf(schedule_fp, " %d,%99s",
                       &stop_id, stop_name) != 2) {
                printf("Invalid schedule data\n");
                fclose(schedule_fp);
                fclose(fp);
                return 1;
            }
            stops[count - 1][i] = stop_id;
            int j = 0;
            while (stop_name[j] != '\0' && j < 50) {
                stop_names[count - 1][i][j] = stop_name[j];
                j++;
            }
            if (stop_name[j] != '\0') {
                printf("Stop name exceeds array capacity\n");
                fclose(schedule_fp); fclose(fp); return 1;
            }
            stop_names[count - 1][i][j] = '\0';
        }
        // Get the route ID and direction from the schedule filename.
        // Check if the filename is correct, then save the route information.
        // Close the schedule file after reading.
        int start = 0;
        for (int p = 0; file_name[p] != '\0'; p++) {
            if (file_name[p] == '/') start = p + 1;
        }
        char prefix[] = "route_";
        for (int p = 0; p < 6; p++) {
            if (file_name[start + p] != prefix[p]) {
                printf("Invalid schedule filename\n");
                fclose(schedule_fp); fclose(fp); return 1;
            }
        }
        int p = start + 6, n = 0;
        while (file_name[p] != '_' && file_name[p] != '\0' && n < 19) {
            route_id[n++] = file_name[p++];
        }
        route_id[n] = '\0';
        if (n == 0 || file_name[p] != '_') {
            printf("Invalid schedule filename\n");
            fclose(schedule_fp); fclose(fp); return 1;
        }
        direction = file_name[p + 1];
        if (direction != 'E' && direction != 'W' && direction != 'N' && direction != 'S') {
            printf("Invalid schedule filename\n");
            fclose(schedule_fp); fclose(fp); return 1;
        }
        int route_index = count - 1;
        int j = 0;
        while (route_id[j] != '\0' && j < 19) {
            route_ids[route_index][j] = route_id[j];
            j++;
        }
        route_ids[route_index][j] = '\0';
        directions[route_index] = direction;
        fclose(schedule_fp);// Close the current schedule file
    }
    fclose(fp);
    // Open the tap history file and check if it exists.
    FILE *tap_fp = fopen(argv[2], "r");
    if (tap_fp == NULL) {
        printf("Error opening tap history\n");
        return 1;
    }
    char input_line[256];
    int consumed = 0;
    // Read the total number of taps and check if it is valid.
    if (!read_line(tap_fp, input_line, sizeof(input_line)) ||
        !read_number(input_line, &consumed, 5, &tap_count) ||
        !only_space(input_line, consumed) ||
        tap_count < 0 || tap_count > MAX_TAPS) {
        printf("Invalid data format\n");
        fclose(tap_fp);
        return 1;
    }
    // Go through each tap record and read its date and time.
    for (int i = 0; i < tap_count; i++) {
        consumed = 0;
        if (!read_line(tap_fp, input_line, sizeof(input_line)) ||
            !read_field(input_line, &consumed, tap_dates[i], 20) ||
            !read_field(input_line, &consumed, tap_times[i], 20)) {
            printf("Invalid data format\n");
            fclose(tap_fp);
            return 1;
        }
        // Check if the tap type is E, X or M.
        tap_types[i] = input_line[consumed];
        if ((tap_types[i] != 'E' && tap_types[i] != 'X' && tap_types[i] != 'M') ||
            input_line[consumed + 1] != ',') {
            printf("Invalid data format\n");
            fclose(tap_fp);
            return 1;
        }
        consumed += 2; //Skip the type character and comma.
        
        // M means the stop is missing, so use -1 for now.
        if (tap_types[i] == 'M') {
            if (input_line[consumed] != '\0') {
                printf("Invalid data format\n");
                fclose(tap_fp);
                return 1;
            }
            tap_stops[i] = -1;
        } else { // For E and X, read the stop ID and check the format.
            if (!read_number(input_line, &consumed, 9, &tap_stops[i]) ||
                !only_space(input_line, consumed)) {
                printf("Invalid data format\n");
                fclose(tap_fp);
                return 1;
            }
        }
    }
    // Check if there is any extra data at the end of the file.
    int extra;
    while ((extra = fgetc(tap_fp)) != EOF) {
        if (extra != ' ' && extra != '\t' && extra != '\r' && extra != '\n') {
            printf("Invalid data format\n");
            fclose(tap_fp);
            return 1;
        }
    }
    // Check if there was any problem reading the file.
    if (ferror(tap_fp)) {
        printf("Invalid data format\n");
        fclose(tap_fp);
        return 1;
    }
    fclose(tap_fp);
    // Create arrays to store the date and time of each tap.
    int tap_years[MAX_TAPS];
    int tap_months[MAX_TAPS];
    int tap_days[MAX_TAPS];
    int tap_hours[MAX_TAPS];
    int tap_minutes[MAX_TAPS];
    for (int i = 0; i < tap_count; i++) {
        char am_pm[3];
        
        // Get the date and time from each tap record.
        if (!read_date(tap_dates[i], &tap_months[i],
                       &tap_days[i], &tap_years[i])) {
            printf("Invalid data format\n");
            return 1;
        }
        if (!read_clock(tap_times[i], &tap_hours[i],
                        &tap_minutes[i], am_pm)) {
            printf("Invalid data format\n");
            return 1;
        }
        // Check if the year, month, day and time are in the correct range.
        if (tap_years[i] < 1 || tap_years[i] > 9999 || tap_months[i] < 1 || tap_months[i] > 12 ||
            tap_days[i] < 1 || tap_hours[i] < 1 || tap_hours[i] > 12 ||
            tap_minutes[i] < 0 || tap_minutes[i] > 59 ||
            !((am_pm[0] == 'A' || am_pm[0] == 'P') && am_pm[1] == 'M')) {
            printf("Invalid data format\n"); return 1;
        }
        // Check the number of days in each month, including leap years.
        int days_in_month = 31;
        if (tap_months[i] == 4 || tap_months[i] == 6 || tap_months[i] == 9 || tap_months[i] == 11) days_in_month = 30;
        if (tap_months[i] == 2) {
            days_in_month = 28;
            if (tap_years[i] % 400 == 0 || (tap_years[i] % 4 == 0 && tap_years[i] % 100 != 0)) days_in_month = 29;
        }
        if (tap_days[i] > days_in_month) { printf("Invalid data format\n"); return 1; }
        // Change am pm time into 24-hour time.
        if (am_pm[0] == 'P' && tap_hours[i] != 12) {
            tap_hours[i] += 12;
        }
        if (am_pm[0] == 'A' && tap_hours[i] == 12) {
            tap_hours[i] = 0;
        }
    }
    // Check if each stop ID exists in the schedule files.
    for (int i = 0; i < tap_count; i++) {
        if (tap_types[i] == 'M') continue;
        int found = 0;
        for (int r = 0; r < count && !found; r++) {
            for (int s = 0; s < stop_counts[r]; s++) {
                if (stops[r][s] == tap_stops[i]) { found = 1; break; }
            }
        }
        if (!found) { printf("Invalid stop ID\n"); return 1; }
    }
    // Convert each tap's date and time into total minutes.
    long long tap_stamp[MAX_TAPS];
    int trip_order[MAX_TAPS];
    int trip_count = 0;
    for (int i = 0; i < tap_count; i++) {
        tap_stamp[i] = day_number(tap_years[i], tap_months[i], tap_days[i]) * 1440
                     + tap_hours[i] * 60 + tap_minutes[i];
    }
    // Find the entry and exit pairs and save their positions.
    for (int i = tap_count - 1; i > 0; i--) {
        if (tap_types[i] == 'E' &&
            (tap_types[i - 1] == 'X' || tap_types[i - 1] == 'M')) {
            if (tap_stamp[i - 1] < tap_stamp[i]) {
                printf("Invalid data format\n");
                return 1;
            }
            trip_order[trip_count++] = i;
        }
    }
    // Check if all taps can be paired into trips.
    if (trip_count * 2 != tap_count) {
        printf("Invalid data format\n");
        return 1;
    }
    // Sort the trips by their start time, so earlier trips are processed first.
    for (int t = 1; t < trip_count; t++) {
        int value = trip_order[t];
        int j = t;
        while (j > 0 && tap_stamp[trip_order[j - 1]] > tap_stamp[value]) {
            trip_order[j] = trip_order[j - 1];
            j--;
        }
        trip_order[j] = value;
    }
    // Open output.txt to save the trip results.
    FILE *output_fp = fopen("output.txt", "w");
    if (output_fp == NULL) {
        printf("Error creating output file\n");
        return 1;
    }
    // Go through each trip and check if the entry and exit taps can be used.
    for (int trip = 0; trip < trip_count; trip++) {
        int i = trip_order[trip];
        if (tap_types[i] == 'E' &&
            (tap_types[i - 1] == 'X' || tap_types[i - 1] == 'M') &&
            tap_used[i] == 0 &&
            tap_used[i - 1] == 0) {
            // Mark taps as used only after a trip is written.
            int entry_stop = tap_stops[i];
            int exit_stop = tap_stops[i - 1];
            int guessed = 0;
            // If the exit tap is missing, try to guess the stop using previous trips.
            // Use the most common exit stop for the same entry stop and hour.
            if (tap_types[i - 1] == 'M') {
                int best_stop = -1;
                int best_frequency = 0;
                for (int k = 0; k < known_count; k++) {
                    if (known_entry_stops[k] != entry_stop ||
                        known_hours[k] != tap_hours[i]) continue;
                    int frequency = known_frequency[k];
                    if (frequency > best_frequency ||
                        (frequency == best_frequency && known_exit_stops[k] < best_stop)) {
                        best_frequency = frequency;
                        best_stop = known_exit_stops[k];
                    }
                }
                if (best_stop < 0) {
                    printf("Cannot guess missing tap\n");
                    fclose(output_fp);
                    return 1;
                }
                exit_stop = best_stop;
                guessed = 1;
            }
            // Calculate how long the trip took in hours and minutes.
            long long duration = tap_stamp[i - 1] - tap_stamp[i];
            long long duration_hours = duration / 60;
            int duration_minutes = (int)(duration % 60);
            int entry_name_position = -1;
            int exit_name_position = -1;
            int matches = 0;
            int selected_route = -1;
            // Check all routes to find which ones contain both the entry and exit stops.
                for (int r = 0; r < count; r++) {
                int entry_position = -1;
                int exit_position = -1;
                
                for (int s = 0; s < stop_counts[r]; s++) {
                    if (stops[r][s] == entry_stop &&
                        entry_position == -1) {
                        entry_position = s;
                    }
                    if (stops[r][s] == exit_stop &&
                        exit_position == -1) {
                        exit_position = s;
                    }
                }
                if (entry_position != -1 &&
                    exit_position != -1) {
                    matching_routes[matches] = r;
                    matches++;
                }
            }
            // If no route matches these stops, show an error.
            if (matches == 0) {
                printf("No matching route\n");
                fclose(output_fp);
                return 1;
            }
            // If only one route matches, use that route.
            if (matches == 1) {
                selected_route = matching_routes[0];
                for (int s = 0; s < stop_counts[selected_route]; s++) {
                    if (stops[selected_route][s] == entry_stop) {
                        entry_name_position = s;
                    }
                    if (stops[selected_route][s] == exit_stop) {
                        exit_name_position = s;
                    }
                }
            }
            // If more than one route matches, show the options and let the user choose.
            if (matches > 1) {
                printf("Ambiguous trip detected:\n");
                printf("Entry: %d (", entry_stop);
                for (int s = 0; s < stop_counts[matching_routes[0]]; s++) {
                    if (stops[matching_routes[0]][s] == entry_stop) {
                        printf("%s", stop_names[matching_routes[0]][s]);
                        break;
                    }
                }
                printf(")\n");
                printf("Exit: %d (", exit_stop);
                for (int s = 0; s < stop_counts[matching_routes[0]]; s++) {
                    if (stops[matching_routes[0]][s] == exit_stop) {
                        printf("%s", stop_names[matching_routes[0]][s]);
                        break;
                    }
                }
                printf(")\n");
                printf("Possible routes:\n");
                for (int j = 0; j < matches; j++) {
                    int r = matching_routes[j];
                    printf("%d. %s %c\n",
                           j + 1,
                           route_ids[r],
                           directions[r]);
                }
                int choice;
                printf("Select route (1-%d): \n", matches);
                if (scanf("%d", &choice) != 1 ||
                    choice < 1 ||
                    choice > matches) {
                    printf("Invalid route choice\n");
                    return 1;
                }
            
                selected_route = matching_routes[choice - 1];
            }
            // Find the stop names for the selected route and write the trip to output.txt.
            if (selected_route != -1) {
                entry_name_position = -1;
                exit_name_position = -1;
                for (int s = 0; s < stop_counts[selected_route]; s++) {
                    if (stops[selected_route][s] == entry_stop) {
                        entry_name_position = s;
                    }
                    if (stops[selected_route][s] == exit_stop) {
                        exit_name_position = s;
                    }
                }
                if (entry_name_position != -1 &&
                    exit_name_position != -1) {
                    fprintf(output_fp,
                                            "%s %c,%s,%s,%04d-%02d-%02dT%02d:%02d:00,%04d-%02d-%02dT%02d:%02d:00,%lld:%02d:00%s\n",
                                            route_ids[selected_route],
                                            directions[selected_route],
                                            stop_names[selected_route][entry_name_position],
                                            stop_names[selected_route][exit_name_position],
                                            tap_years[i],
                                            tap_months[i],
                                            tap_days[i],
                                            tap_hours[i],
                                            tap_minutes[i],
                                            tap_years[i - 1],
                                            tap_months[i - 1],
                                            tap_days[i - 1],
                                            tap_hours[i - 1],
                                            tap_minutes[i - 1],
                                            duration_hours,
                                            duration_minutes,
                                            guessed ? ",Guessed" : "");
                    // Mark these two taps as used so they are not processed again.
                    tap_used[i] = 1;
                    tap_used[i - 1] = 1;
                    // Save information from normal trips to help guess missing exit stops later.
                    if (tap_types[i - 1] == 'X') {
                    
                        int bucket = -1;
                        for (int k = 0; k < known_count; k++) {
                            if (known_entry_stops[k] == entry_stop &&
                                known_hours[k] == tap_hours[i] &&
                                known_exit_stops[k] == exit_stop) {
                                bucket = k;
                                break;
                            }
                        }
                        if (bucket == -1) {
                            bucket = known_count++;
                            known_entry_stops[bucket] = entry_stop;
                            known_exit_stops[bucket] = exit_stop;
                            known_hours[bucket] = tap_hours[i];
                            known_frequency[bucket] = 0;
                        }
                        known_frequency[bucket]++;
                    }
                }
            }
        }
        }
    // Check for any unused taps and print a message if an entry or exit is missing.
        for (int i = tap_count - 1; i >= 0; i--) {
            if (tap_used[i] == 0) {
                if (tap_types[i] == 'E') {
                    printf("Missing exit tap after stop %d\n",
                           tap_stops[i]);
                } else if (tap_types[i] == 'X') {
                    printf("Missing entry tap before stop %d\n",
                           tap_stops[i]);
                }
            }
        }
        fclose(output_fp); // finally close the output file and finish the program.
        return 0;
    }
