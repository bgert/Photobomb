//
//  Ben Gertz
//  Photobomb
//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <dirent.h>

int** add_file_to_master(char* fname, int current_file_number, int fsize);
int* size_of_photo(char* fname, int* size_of_p_val);
int mode(int lst[], int num_ele);



int size_of_open_folder(int folder_size, char* fname, int size_of_p_val, int* colums_rows){
    //declares a intermediate list of integers to get the return value of size_of photo
    //I then assign the intermediate list to colums_rows becuase I couldn't get the pointer to work
    //any other way
    int* intermediate;
    
    DIR* folder;
    struct dirent *files;
    folder = opendir(fname);
    
    if (folder == NULL) {
        printf("could not open folder photos\n");
        return -1;
    }
    //This reads thru the directory and increments for every folder to find out the number of photos in a directory
    //It is told to ignore hidden files beginning with ., itself, and the a.out file created when running the program
    while((files = readdir(folder)) != NULL){
        if ((files->d_name[0] != '.') && (strcmp(files-> d_name, "main.c")!=0) && (strcmp(files-> d_name, "a.out")!= 0)){
            
            folder_size++;
            
        }
    }
    //resets directory stream to top
    rewinddir(folder);
    //for every file in the directory this is called to determine the size of the photo
    while((files = readdir(folder)) != NULL){
        if ((files->d_name[0] != '.') && (strcmp(files-> d_name, "main.c")!=0) && (strcmp(files-> d_name, "a.out")!= 0)){
            
            
            
            intermediate = size_of_photo(files->d_name, &size_of_p_val);
            break;
            
        }
    }
    colums_rows[0] = intermediate[0];
    colums_rows[1] = intermediate[1];
    
    closedir(folder);
    return folder_size;
}
int*** open_folder(char* fname, int fsize, int* size_of_p_val){
    
    
    struct dirent *files;
    
    DIR* folder;
    folder = opendir(fname);
    int current_file_number = 0;
    //creates a 3d array to hold the contents of each file
    int*** m = (int***)malloc(fsize * sizeof(sizeof(size_of_p_val)));
    while((files = readdir(folder))!= NULL){
        if ((files->d_name[0] != '.') && (strcmp(files-> d_name, "main.c")!=0) && (strcmp(files-> d_name, "a.out")!= 0)){
            
            //calls add file to master to go and read in a file to a 2d array then return it
            
            m[current_file_number] = add_file_to_master(files->d_name, current_file_number, fsize);
            ++current_file_number;
            
        }
    }
    closedir(folder);
    //returns the 3d array made in this function to the array in main
    return m;
}
int* size_of_photo(char* fname, int* size_of_p_val){
    //this program determines the size of the photo only so that its information can be returned to the main
    //and used for allocating memory to hold the photos
    char encoding[2];
    static int c_r[2];
    
    FILE* inFile = NULL;
    int columns;
    int rows;
    int max_color_val;
    int** arr_photo;
    inFile = fopen(fname, "r");
    
    fscanf(inFile, "%s %d %d %d", encoding, &columns, &rows, &max_color_val);
    
    //this creates an containing the number of columns and rows in the ppm file
    c_r[0] = columns;
    c_r[1] = rows;
    
    fclose(inFile);
    return c_r;
}

int** add_file_to_master(char* fname, int current_file_number, int fsize){
    char encoding[2];
    FILE* inFile = NULL;
    //This reads in the file values, then it creates a 2d array for a given file then returns it to be added to a list in open_folder
    int columns;
    int rows;
    int max_color_val;
    int** arr_photo;
    inFile = fopen(fname, "r");
    
    fscanf(inFile, "%s %d %d %d", encoding, &columns, &rows, &max_color_val);
    
    //allocates memory for a 2d array and then allocates memory for the arrays within each element of the 2d array

    arr_photo = (int**)malloc(sizeof(int*) * rows * columns * 3);
    for(int i = 0; i < (rows * columns); ++i){
        arr_photo[i] = (int*) malloc(3 * sizeof(int));
    }
    
    
    
    for(int i=0; i < (columns * rows); i++){
        
        fscanf(inFile, "%d %d %d", &arr_photo[i][0], &arr_photo[i][1], &arr_photo[i][2]);
    }
    
    fclose(inFile);
    return arr_photo;
}
int** math_it(int*** master_lst, int num_pxls, int folder_size){
    
    //There are arrays for each color value that match the size of the amount of files in a folder
    int red[folder_size];
    int green[folder_size];
    int blue[folder_size];
    //this creates a 2d array that is the finished photo
    int** fin_photo = (int**)malloc(sizeof(int*) *num_pxls* 3);
    
    for(int i = 0; i < num_pxls; i++){
        fin_photo[i] = (int*) malloc(3 * sizeof(int));
    }
    //for each triplet value in a file I take the RGB values for every file and add it to the list
    //using the mode function i find the most common value and return it to fill the given slot in fin_photo
    for(int triplet = 0; triplet < num_pxls; triplet++){
        for(int photo = 0; photo < folder_size; photo++){
            red[photo] = master_lst[photo][triplet][0];
            green[photo] = master_lst[photo][triplet][1];
            blue[photo] = master_lst[photo][triplet][2];
        }
        fin_photo[triplet][0] = mode(red, folder_size);
        fin_photo[triplet][1] = mode(green, folder_size);
        fin_photo[triplet][2] = mode(blue, folder_size);
    }
    
    return fin_photo;
    
}

//finds mode of a list of elements
int mode(int lst[], int num_ele) {
    int maxValue = 0;
    int maxCount = 0;
    
    for (int i = 0; i < num_ele; ++i) {
        int count = 0;
        
        for (int j = 0; j < num_ele; ++j) {
            if (lst[j] == lst[i])
                ++count;
        }
        
        if (count > maxCount) {
            maxCount = count;
            maxValue = lst[i];
        }
    }
    
    return maxValue;
}

int main() {
    //this is the file name for the file that is outputted
    char out_path[11] = "output.ppm";
    int folder_size = 0;
    //this will be given the file directory the user inputs
    char fname[100];
    //this is the memory size of one triplet to be used for memory allocation
    int size_of_p_val = 0;
    //this is the number of pixels in one image
    int num_pxls;
    //This array holds the value for columns and rows. I know i spelled colums wrong but i dont want to make errors trying to correct it
    int colums_rows[2];
    //in order to allow the user to enter the file path then add the output.ppm extension to it I need to add a / to the end of fname
    //this allows me to do so
    char slash_char[1] = "/";
    printf("please enter the full file path of the folder you want to get photos from\n");
    scanf("%s", fname);    
    
    folder_size = size_of_open_folder(folder_size, fname, size_of_p_val, colums_rows);
    
    int*** master;
    num_pxls = colums_rows[0] * colums_rows[1];
    master = open_folder(fname, folder_size, &size_of_p_val);
    
    //allocates memory for array that is the photo to be exported
    int** out_photo = (int**)malloc(sizeof(int*) *num_pxls* 3);
    //takes in the master list and comes up with the fixed photo to be exported
    out_photo = math_it(master, num_pxls, folder_size);
    
    //adds the slash and the output.ppm extension to fname
    strcat(fname, slash_char);
    strcat(fname, out_path);
    //open output.ppm
    FILE *output = fopen(fname, "w");
    
    //print encoding to output.ppm
    fprintf(output, "P3 %d %d 255 ", colums_rows[0], colums_rows[1]);
    //print data to output.ppm
    for(int i = 0; i < num_pxls; i++){
        fprintf(output, "%d %d %d ", out_photo[i][0], out_photo[i][1], out_photo[i][2]);
    }
    printf("Your file is called output.txt and is in the same directory you gave the program\n");
    fclose(output);
    
}
