/**
 * @file 	MidiTypes.h
 *
 * @date 	22.9.2023
 * @author 	Ondrej Slaby <slaby@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#ifndef MIDITYPES_H_
#define MIDITYPES_H_

#include <cstdint>
#include <string>
namespace iimavlib {

namespace midi {

struct midi_info_t {
    std::string name;
};

struct note_t {
    uint8_t channel;
    uint8_t note;
    uint8_t velocity;
};

struct control_t {
    uint8_t channel;
    uint8_t param;
    uint8_t value;
};

enum class note_name : uint8_t
{
    // n1 = -1 (Negative), Csh = C# (SHarp)
    C_n1, Csh_n1, D_n1, Dsh_n1, E_n1, F_n1, Fsh_n1, G_n1, Gsh_n1, A_n1, Ash_n1, B_n1,
    C_0, Csh_0, D_0, Dsh_0, E_0, F_0, Fsh_0, G_0, Gsh_0, A_0, Ash_0, B_0,
    C_1, Csh_1, D_1, Dsh_1, E_1, F_1, Fsh_1, G_1, Gsh_1, A_1, Ash_1, B_1,
    C_2, Csh_2, D_2, Dsh_2, E_2, F_2, Fsh_2, G_2, Gsh_2, A_2, Ash_2, B_2,
    C_3, Csh_3, D_3, Dsh_3, E_3, F_3, Fsh_3, G_3, Gsh_3, A_3, Ash_3, B_3,
    C_4, Csh_4, D_4, Dsh_4, E_4, F_4, Fsh_4, G_4, Gsh_4, A_4, Ash_4, B_4,
    C_5, Csh_5, D_5, Dsh_5, E_5, F_5, Fsh_5, G_5, Gsh_5, A_5, Ash_5, B_5,
    C_6, Csh_6, D_6, Dsh_6, E_6, F_6, Fsh_6, G_6, Gsh_6, A_6, Ash_6, B_6,
    C_7, Csh_7, D_7, Dsh_7, E_7, F_7, Fsh_7, G_7, Gsh_7, A_7, Ash_7, B_7,
    C_8, Csh_8, D_8, Dsh_8, E_8, F_8, Fsh_8, G_8, Gsh_8, A_8, Ash_8, B_8,
    C_9, Csh_9, D_9, Dsh_9, E_9, F_9, Fsh_9, G_9,
};

inline note_name note_to_name(const uint8_t note)
{
    return static_cast<note_name>(std::min<uint8_t>(127u, note));
}

inline uint8_t name_to_note(const note_name name)
{
    return static_cast<uint8_t>(name);
}

}

}

#endif //MIDITYPES_H_