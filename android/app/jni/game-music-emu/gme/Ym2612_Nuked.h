


#ifndef YM2612_EMU_H
#define YM2612_EMU_H

typedef void Ym2612_Nuked_Impl;

class Ym2612_Nuked_Emu  {
	Ym2612_Nuked_Impl* impl;
	double prev_sample_rate;
	double prev_clock_rate;
public:
	Ym2612_Nuked_Emu();
	~Ym2612_Nuked_Emu();



	const char* set_rate( double sample_rate, double clock_rate );


	void reset();


	enum { channel_count = 6 };
	void mute_voices( int mask );


	void write0( int addr, int data );


	void write1( int addr, int data );


	typedef short sample_t;
	enum { out_chan_count = 2 };  
	void run( int pair_count, sample_t* out );
};

#endif

