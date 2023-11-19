// main.cpp
// enigma project
// Colin Taylor
// last edited: 22/11/21
// implements an encryption/decryption algorithm for an enigma machine style encryption algorithm
// enigma machine plugboard, rotor and reflector specified from command line input files
// letter input and output to/from standard i/o stream

/*  INPUT SYNTAX: ./enigma ./plugboards/I.pb ./reflectors/I.rf ./rotors/I.rot ./rotors/II.rot ./rotors/III.rot ./rotors/I.pos
    // char **argv //
    [0]             :   ./enigma
    [1]             :   ./plugboards/I.pb
    [2]             :   ./reflectors/I.rf
    [3 -> argc-2]   :   ./rotors/I.rot ./rotors/II.rot ./rotors/III.rot etc.
    [argc-1]        :   ./rotors/I.pos
*/

#include "enigma.h"

using namespace std;

int main(int argc, char **argv) {

    if (argc < 4) {
        cerr << "Insufficient number of program parameters. Aborting program.\n";
        return INSUFFICIENT_NUMBER_OF_PARAMETERS;
    }

    int error_code = NO_ERROR;

    // configuration //

    char *rotorunit_filenames[argc-2];
    rotorunit_filenames[0] = argv[2]; // reflector
    for (int i = 3; i < argc-1; i++) {
        rotorunit_filenames[i-2] = argv[i]; // rotors
    }
    rotorunit_filenames[argc-3] = argv[argc-1]; // rotor positions

    Plugboard plugboard;
    error_code = plugboard.config(argv[1]);
    if (error_code) return error_code;

    Rotor_unit rotor_unit;
    error_code = rotor_unit.config(rotorunit_filenames, argc);
    if (error_code) return error_code;

    // action //

    char input_letter = 'A';
    error_code = take_input(input_letter);
    while(!error_code) {
        // rotate rightmost rotor (and any knock-ons) before mapping
        rotor_unit.rotate_to_my_left(nullptr);
        // map letter
        display_letter(plugboard.map_letter(rotor_unit.map_letter(plugboard.map_letter(input_letter))));
        error_code = take_input(input_letter);
    }

    return error_code;
}