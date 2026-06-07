
import csv
import matplotlib.pyplot as plt
import numpy as np

import filter

# plot amplitudes in time domain
def plot_td( ts, ds, xlabel="time", ylabel="ampl", title="" ):
	plt.plot( ts, ds, "b-" )
	plt.xlabel( xlabel )
	plt.ylabel( ylabel )
	plt.show()

# plot amplitudes in frequency domain
def plot_fd( ts, ds, xlabel="freq", ylabel="ampl", title="" ):
	plt.loglog( ts, abs( ds ), "b" ) 
	plt.xlabel( xlabel )
	plt.ylabel( ylabel )
	plt.show()

# plot amplitudes in both domains 
def plot_td_fd( ts, ds_td, freqs, ds_fd ):
	fig, ( ax1, ax2 ) = plt.subplots( 2, 1 )
	ax1.plot( ts, ds_td, "b" )
	ax1.set_xlabel( "time" )
	ax1.set_ylabel( "ampl" )
	ax2.loglog( freqs, abs( ds_fd ), "b" )
	ax2.set_xlabel( "freq" )
	ax2.set_ylabel( "|ampl|" )
	plt.show()

def run_maf( cnt, data, ts, tap=32 ):
	maf = filter.MAF( tap=32 )

	maf_out = [ 0.0 ] * cnt
	for i in range( cnt ):
		maf.add_sample( data[ i ] )
		maf_out[ i ] = maf.get_avg()

	fft_out = np.fft.fft( maf_out ) / cnt
	freqs = np.arange( cnt ) / tmax
	fft_out = fft_out[ range( cnt // 2 ) ]
	freqs = freqs[ range( cnt // 2 ) ]

	plot_td_fd( ts, data, freqs, fft_out )


def run_iir( cnt, data, ts, A=0.999 ):
	iir = filter.IIR( A=A )

	iir_out = [ 0.0 ] * cnt
	for i in range( cnt ):
		iir.add_sample( data[ i ] )
		iir_out[ i ] = iir.get_avg()

	fft_out = np.fft.fft( iir_out ) / cnt
	freqs = np.arange( cnt ) / tmax
	fft_out = fft_out[ range( cnt // 2 ) ]
	freqs = freqs[ range( cnt // 2 ) ]

	plot_td_fd( ts, data, freqs, fft_out )

def run_fir( cnt, data, ts, coefs ):
	fir = filter.FIR( coefs=coefs )

	fir_out = [ 0.0 ] * cnt
	for i in range( cnt ):
		fir.add_sample( data[ i ] )
		fir_out[ i ] = fir.get_avg()

	fft_out = np.fft.fft( fir_out ) / cnt
	freqs = np.arange( cnt ) / tmax
	fft_out = fft_out[ range( cnt // 2 ) ]
	freqs = freqs[ range( cnt // 2 ) ]

	plot_td_fd( ts, data, freqs, fft_out )

# sampling rate 400Hz, cutoff 10Hz, transition BW 50Hz
coefs_D = [
    0.003406212229674561,
    0.004615070664811991,
    0.007608634521169020,
    0.012659850008542091,
    0.019798608584983383,
    0.028784787004261951,
    0.039116212018982140,
    0.050071944217781210,
    0.060786348627844503,
    0.070345019112866947,
    0.077890394025478593,
    0.082723325067391634,
    0.084387187832423882,
    0.082723325067391634,
    0.077890394025478607,
    0.070345019112866961,
    0.060786348627844530,
    0.050071944217781210,
    0.039116212018982154,
    0.028784787004261965,
    0.019798608584983383,
    0.012659850008542096,
    0.007608634521169026,
    0.004615070664811996,
    0.003406212229674561,
]

if __name__ == "__main__":

	PATH = "sigA.csv"

	data = []
	cnt = 0
	tmax = 0.0
	with open( PATH, mode="r" ) as f:
		reader = csv.reader( f )
		for row in reader:
			data.append( float( row[ 1 ] ) )
			tmax = float( row[ 0 ] )

		cnt = len( data )

	sampling_rate = cnt / tmax
	print( f"Sampling rate: { sampling_rate } Hz" )
	ts = np.arange( cnt )

	#plot_data( ts, data, title=PATH )

	fft_out = np.fft.fft( data ) / cnt
	freqs = np.arange( cnt ) / tmax
	fft_out = fft_out[ range( cnt // 2 ) ]
	freqs = freqs[ range( cnt // 2 ) ]

	#plot_loglog( freqs, fft_out )

	plot_td_fd( ts, data, freqs, fft_out )

	run_maf( cnt, data, ts, tap=32 )
	#run_iir( cnt, data, ts, A=0.995 )

