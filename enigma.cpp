// enigma.cpp
// enigma project
// Colin Taylor
// last edited: 22/11/21

// Class member function definitions

// Sources used for this file:
/*
while (plugboard_file >> letter1 >> letter2)
: https://stackoverflow.com/questions/43956124/c-while-loop-to-read-from-input-file?rq=1
peek()
: https://www.cplusplus.com/reference/istream/istream/peek/
ASCII table
: https://www.rapidtables.com/code/text/ascii-table.html
*/

#include "enigma.h"

using namespace std;

int take_input(char& letter) { 
    char input;
    cin >> input;
    if (!(input>='A' && input<='Z')) {
        cerr << "Invalid input character! Exiting program.\n";
        return INVALID_INPUT_CHARACTER;
    }
    else {
        letter = input;
        return NO_ERROR;
    }
}

void display_letter(char letter) {
    cout << letter;
}

// helper function defs //


// only returns ints between 0-25 any other chars cause exit of program
int take_int_from_0_to_25_from_fstream(ifstream& file) {
    int i = 0;
    file >> ws;
    file.peek(); // sets filestream to false at eof without reading it
    if (!file) {
        return -1;
    }
    if (file.peek() < '0' || file.peek() > '9' ) {
        cerr << "Non-numeric character in plugboard file. Exiting program." << endl;
        exit(NON_NUMERIC_CHARACTER);
    }
    file >> i;
    if (i < 0 || i > 25) {
        cerr << "Invalid index. Exiting program." << endl;
        exit(INVALID_INDEX);
    }
    else return i;
}

int check_input_file_stream_is_good(ifstream const& input_file_stream) {
    if (!input_file_stream) {
        cerr << "Failed to open file! Exiting program." << endl;
        return ERROR_OPENING_CONFIGURATION_FILE;
    }
    else return NO_ERROR;
}

void initialise_int_array(int* int_array, int const size_of_array) {
    for (int i = 0; i < size_of_array; i++) { 
        int_array[i] = i; 
    }
}


// Class member function defs //


// Keyboard class function defs

bool Keyboard::take_input(char& letter) { 
    char input;
    cin >> input;
    if (input == '0') {
        cout << "End of program." << endl;
        return false;
    }
    if (!(input>='A' && input<='Z')) {
        cerr << "Invalid input character! Exiting program" << endl;
        exit(INVALID_INPUT_CHARACTER);
    }
    else {
        letter = input;
        return true;
    }
}


// Plugboard class function defs //

int Plugboard::config(char* filename) {
    int error_code = 0;
    initialise_int_array(plug_cables, LENGTH_OF_ALPHABET);
    ifstream plugboard_file(filename);
    error_code = check_input_file_stream_is_good(plugboard_file);
    if (error_code) return error_code;
    int letter1, letter2, count = 0;
    while (true) {
        letter1 = take_int_from_0_to_25_from_fstream(plugboard_file);
        if (!plugboard_file) break;
        else count++; // count the number only if input was taken
        letter2 = take_int_from_0_to_25_from_fstream(plugboard_file);
        if (!plugboard_file) break;
        else count++;
        if (letter1 == letter2) {
            cerr << "Plugboard cannot map a contact to itself! Exiting program." << endl;
            return IMPOSSIBLE_PLUGBOARD_CONFIGURATION;
        }
        // non-default value indicates multiple connections
        if ((plug_cables[letter1] != letter1) || (plug_cables[letter2] != letter2)) {
            cerr << "Plugboard cannot map a contact to more than one contact! Exiting program." << endl;
            return IMPOSSIBLE_PLUGBOARD_CONFIGURATION;
        }
        plug_cables[letter1] = letter2;
        plug_cables[letter2] = letter1;
    }
    if (count%2 == 1) {
        cerr << "Incorrect number of plugboard parameters! Exiting program." << endl;
        return INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS;
    }
    plugboard_file.close();
    return NO_ERROR;
}

char Plugboard::map_letter(char input_letter) { 
    return ((plug_cables[input_letter-'A'])+'A');
}


// Rotor class function defs //

int Rotor::config(char* filename) {
    int error_code = 0;
    ifstream rotor_file(filename);
    error_code = check_input_file_stream_is_good(rotor_file);
    if (error_code) return error_code;
    bool check_array[LENGTH_OF_ALPHABET] = {};
    for (int i = 0; i < LENGTH_OF_ALPHABET; i++) { 
            right_to_left_connections[i] = take_int_from_0_to_25_from_fstream(rotor_file);
            // check letter has not been mapped to already
            if (check_array[right_to_left_connections[i]] == true) {
                cerr << "Invalid rotor mapping two inputs to the same output. Exiting program." << endl;
                return INVALID_ROTOR_MAPPING;
            }
            // mark this letter as mapped for checking later
            check_array[right_to_left_connections[i]] = true;
            // set asymmetric mapping array
            left_to_right_connections[right_to_left_connections[i]] = i;
        }
    // check all letters are mapped to
    for (int i = 0; i < LENGTH_OF_ALPHABET; i++) { 
        if (check_array[i] == false) {
            cerr << "Invalid rotor does not map to all output letters. Exiting program." << endl;
            return INVALID_ROTOR_MAPPING;
        }
    }
    int notch = 0;
    while (true) {
            notch = take_int_from_0_to_25_from_fstream(rotor_file);
            if (!rotor_file) break;
            else notches[notch] = 1;
        }
    rotor_file.close();
    return NO_ERROR;
}

char Rotor::map_right_to_left(char input_letter) {
    return right_to_left_connections[input_letter-'A']+'A';
}

char Rotor::map_left_to_right(char input_letter) {
    return left_to_right_connections[input_letter-'A']+'A';
}

void Rotor::set_initial_rotation(int const rotation) {
    for (int i = 0; i < rotation; i++) {
        this->rotate_me(true);
    }
}

// use this for both initial setup and rotation during use of machine
// hence the bool parameter
void Rotor::rotate_me(bool const initial_setup) {
    this->rotation++; // for checking notch positions only
    if (!initial_setup) {
        if (notches[rotation]==1) rotor_unit->rotate_to_my_left(this);
    }
    int right_to_left_copy[LENGTH_OF_ALPHABET] = {};
    for (int i = 0; i < LENGTH_OF_ALPHABET; i++) {
        right_to_left_copy[i] = right_to_left_connections[i];
    }
    // elements are shifted one to the left and their value reduced by 1 on each rotation (with -1 wrapping round to LENGTH_OF_ALPHABET-1)
    // can then set left_to_right_connections as before but from new values
    for (int i = 0; i < LENGTH_OF_ALPHABET; i++) {
        right_to_left_connections[i] = (right_to_left_copy[(i+LENGTH_OF_ALPHABET+1)%LENGTH_OF_ALPHABET]+LENGTH_OF_ALPHABET-1)%LENGTH_OF_ALPHABET;
        left_to_right_connections[right_to_left_connections[i]] = i;
    }
}

void Rotor::set_rotor_unit_pointer(Rotor_unit* rotor_unit) {
    this->rotor_unit = rotor_unit;
}


// Reflector class functions defs //


int Reflector::config(char* filename) {
    int error_code = 0;
    initialise_int_array(reflector_connections, LENGTH_OF_ALPHABET);
    ifstream reflector_file(filename);
    error_code = check_input_file_stream_is_good(reflector_file);
    if (error_code) return error_code;
    int letter1, letter2;
    int count = 0;
    while (true) {
        letter1 = take_int_from_0_to_25_from_fstream(reflector_file);
        if (!reflector_file) break;
        else count++;       
        letter2 = take_int_from_0_to_25_from_fstream(reflector_file);
        if (!reflector_file) break;
        else count++;
        if (letter1 == letter2) {
            cerr << "Reflector cannot map a contact to itself! Exiting program." << endl;
            return NON_NUMERIC_CHARACTER;
        }
        // non-default value indicates multiple connections
        if ((reflector_connections[letter1] != letter1) || (reflector_connections[letter2] != letter2)) {
            cerr << "Reflector cannot map a contact to more than one contact! Exiting program." << endl;
            return INVALID_REFLECTOR_MAPPING;
        }
        reflector_connections[letter1] = letter2;
        reflector_connections[letter2] = letter1;
    }
    if (count != LENGTH_OF_ALPHABET) {
        cerr << "Incorrect number of reflector parameters! Exiting program." << endl;
        return INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS;
    }
    reflector_file.close();
    return NO_ERROR;
}

char Reflector::map_letter(char input_letter) {
    return ((reflector_connections[input_letter-'A'])+'A');
}


// Rotor_unit class function defs //


Rotor_unit::~Rotor_unit() {
        if (rotor_array != nullptr) {
            delete[] rotor_array;
        }
        if (reflector != nullptr) {
            delete reflector;
        }
    }

int Rotor_unit::config_rotor_starting_rotations(char *filename, int const argc) {
    int error_code = 0;
    int rotation = 0;
    ifstream initial_rotations_file(filename);
    error_code = check_input_file_stream_is_good(initial_rotations_file);
    if (error_code) return error_code;
    int count = 0;
    for (int i = 1; i < argc - 3; i++) {
        rotation = take_int_from_0_to_25_from_fstream(initial_rotations_file);
        if (!initial_rotations_file) break;
        rotor_array[i-1].set_initial_rotation(rotation);
        count++;
    }
    if (count != argc - 4) return NO_ROTOR_STARTING_POSITION;
    initial_rotations_file.close();
    return NO_ERROR;
}

int Rotor_unit::config(char **filenames, int const argc) {
    int error_code = 0;
    this->number_of_rotors = argc - 4;
    this->reflector = new Reflector;
    error_code = reflector->config(filenames[0]);
    if (error_code) return error_code;
    this->rotor_array = new Rotor[argc-4];
    for (int i = 1; i < argc - 3; i++) {
        error_code = rotor_array[i-1].config(filenames[i]);
        if (error_code) return error_code;
        rotor_array[i-1].set_rotor_unit_pointer(this);
    }  
    error_code = this->config_rotor_starting_rotations(filenames[argc-3], argc);
    if (error_code) return error_code;
    return NO_ERROR;
}

char Rotor_unit::map_letter(char input_letter) {
    // map through rotors from right to left first (backwards through array)
    for (int i = number_of_rotors-1; i>=0; i--) {
        input_letter = rotor_array[i].map_right_to_left(input_letter);
    }
    // then map through reflector
    input_letter = reflector->map_letter(input_letter);
    // finally map back through rotors left to right (forward through array)
    for (int i = 0; i < number_of_rotors; i++) {
        input_letter = rotor_array[i].map_left_to_right(input_letter);
    }
    return input_letter;
}

void Rotor_unit::rotate_to_my_left(Rotor* rotor) {
    if (rotor == nullptr) {
        rotor_array[number_of_rotors-1].rotate_me(false);
    }
    // find rotor in array and tell one to the left to rotate if not already at far left
    else for (int i = 0; i < number_of_rotors; i++) {
        if (&rotor_array[i] == rotor) {
            if (i > 0) {
                rotor_array[i-1].rotate_me(false);
            }
        }
    }
}


// Lightboard class function defs

void Lightboard::display_letter(char letter) {
    cout << letter;
}
