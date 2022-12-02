#pragma once

class Player {
    public:
        float assets;

        Player();
        void gain(float gain);
        void lose(float loss);
        float get_assets();
};

// class Defender : public Player {
//     public:
//         static int ctr;
//         int id;

//         Defender();
// }