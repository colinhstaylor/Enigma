// engima.h
// enigma project
// Colin Taylor
// last edited: 22/11/21

// class defintions and function declarations

// enigma machine made up of one plugboard and one rotor_unit instance
// order of mapping functions implemented in main

#pragma once
#include<iostream>
#include<fstream>
#include<string>
#include "errors.h"

// Global constants //

int const LENGTH_OF_ALPHABET = 26;

int take_input(char& letter);

void display_letter(char letter);

// Helper functions //


// tests the input file for good, valid int characters '0' -> '25'
int take_int_from_0_to_25_from_fstream(std::ifstream& file);

// checks file stream is good
int check_input_file_stream_is_good(std::ifstream const& input_file_stream);

// sets array[i] = i throughout the array
void initialise_int_array(int* int_array, int const size_of_array);


// Classes //


// class takes input letters
class Keyboard {
public:
    bool take_input(char& letter);
};

// class configures, stores and maps the plugboard in the enigma machine
class Plugboard {
    int plug_cables[LENGTH_OF_ALPHABET] = {};
public:
    // configures the plugboard settings from command line argument
    int config(char* filename);
    // maps a letter through the plugboard
    char map_letter(char input_letter);
};

// forward declaration for use in Rotor class; description with full declaration below
class Rotor_unit;

// class for storing one rotor
// stores mapping and rotation information for one rotor
class Rotor {
    //asymmetric mapping function hence two arrays and functions below
    int right_to_left_connections[LENGTH_OF_ALPHABET] = {};
    int left_to_right_connections[LENGTH_OF_ALPHABET] = {}; 
    int notches[LENGTH_OF_ALPHABET] = {};
    // rotation variable for checking for notch positions
    int rotation = 0;
    // for sending rotation signals back through rotor_unit to left rotor
    Rotor_unit* rotor_unit = nullptr;
public:
    // configures the rotor settings from command line argument
    int config(char* filename);
    // function to map from input from the right passing to its left
    char map_right_to_left(char input_letter);
    // function for mapping input from left out to rotor's right
    char map_left_to_right(char input_letter);
    // calls rotate me with initial_setup = true
    // called on Rotor_unit::config from cmd line arguments
    void set_initial_rotation(int const rotation);
    // rotates the rotor and adjusts mapping arrays
    // tells rotor_unit to rotate next rotor to the left when notch position is reached
    void rotate_me(bool const initial_setup);
    // assigns rotor_unit pointer when initialised by rotor unit config fnc
    void set_rotor_unit_pointer(Rotor_unit* rotor_unit);
};

// stores reflector configuration and mapping function
class Reflector {
    int reflector_connections[LENGTH_OF_ALPHABET] = {};
public:
    // configures the reflector settings from command line argument
    int config(char* filename);
    // maps a letter according to reflector connections
    char map_letter(char input_letter);
};

// class stores the rotors and reflector setup of the enigma machine supplied in command line
// controls the passing of inputs and outputs between rotors and with reflector
class Rotor_unit {
    // member objects created in dynamic memory by Rotor_unit::config
    // and initialised by Rotor::config and Reflector::config
    Rotor *rotor_array = nullptr;
    int number_of_rotors = 0;
    Reflector* reflector = nullptr;
public:
    // delete rotors and reflector if configured (dynamic config fnc)
    ~Rotor_unit();
    // configures starting positions of rotor members from cmd line argument
    int config_rotor_starting_rotations(char* filename, int const argc);
    // activates config functions for elements of rotor_array and refelctor member objects from command line argument
    // filenames: {plugboard, reflector, rotor1, rotor2, ..., rotorN, rotor intial rotations}
    int config(char** filenames, int const argc);
    // send letters to mapping member object functions in order and return final mapping (to be sent to plugboard by main)
    char map_letter(char input_letter);
    // parameter = nullptr to indicate singal comes from plugboard
    // parameter = this when signal coming from rotor
    void rotate_to_my_left(Rotor* rotor);
};

// class to display output of enigma machine
class Lightboard {
public:
    void display_letter(char letter);
};
