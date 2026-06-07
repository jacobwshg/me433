
import csv
import matplotlib.pyplot as plt
import numpy as np

import filter
from fir_coefs import FIR_DES, FIR_COEFS

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
def plot_td_fd( ts, ds_td, freqs, ds_fd, figname="", title="" ):
	fig, ( ax1, ax2 ) = plt.subplots( 2, 1 )
	ax1.plot( ts, ds_td, "b" )
	ax1.set_xlabel( "time" )
	ax1.set_ylabel( "ampl" )
	ax2.loglog( freqs, abs( ds_fd ), "b" )
	ax2.set_xlabel( "freq" )
	ax2.set_ylabel( "|ampl|" )
	plt.suptitle( title )
	plt.savefig( figname )
	plt.show()

# plot both raw and filtered amplitudes in both domains 
def plot_td_fd_cmp(
	ts, raw_td, filtered_td,
	freqs, raw_fd, filtered_fd,
	figname="", title=""
):
	fig, ( ax1, ax2 ) = plt.subplots( 2, 1 )
	ax1.plot( ts, raw_td, "black" )
	ax1.plot( ts, filtered_td, "red" )
	ax1.set_xlabel( "time" )
	ax1.set_ylabel( "ampl" )
	ax2.loglog( freqs, abs( raw_fd ), "black" )
	ax2.loglog( freqs, abs( filtered_fd ), "red" )
	ax2.set_xlabel( "freq" )
	ax2.set_ylabel( "|ampl|" )
	plt.suptitle( title )
	plt.savefig( figname )
	plt.show()


def run_maf(
	cnt, data, ts, fft_out, tap=64,
	figname="maf.png", title=""
):
	maf = filter.MAF( tap=32 )

	maf_out = [ 0.0 ] * cnt
	for i in range( cnt ):
		maf.add_sample( data[ i ] )
		maf_out[ i ] = maf.get_avg()

	maf_fft_out = np.fft.fft( maf_out ) / cnt
	freqs = np.arange( cnt ) / tmax
	maf_fft_out = maf_fft_out[ range( cnt // 2 ) ]
	freqs = freqs[ range( cnt // 2 ) ]

	#plot_td( ts, maf_out )

	plot_td_fd_cmp(
		ts, data, maf_out,
		freqs, fft_out, maf_fft_out,
		figname=figname, title=title
	)


def run_iir(
	cnt, data, ts, fft_out, A=0.999,
	figname="iir.png", title=""
):
	iir = filter.IIR( A=A )

	iir_out = [ 0.0 ] * cnt
	for i in range( cnt ):
		iir.add_sample( data[ i ] )
		iir_out[ i ] = iir.get_avg()

	iir_fft_out = np.fft.fft( iir_out ) / cnt
	freqs = np.arange( cnt ) / tmax
	iir_fft_out = iir_fft_out[ range( cnt // 2 ) ]
	freqs = freqs[ range( cnt // 2 ) ]

	plot_td_fd_cmp(
		ts, data, iir_out,
		freqs, fft_out, iir_fft_out,
		figname=figname, title=title
	)


def run_fir(
	cnt, data, ts, fft_out, coefs,
	figname="fir.png", title=""
):
	fir = filter.FIR( coefs=coefs )

	fir_out = [ 0.0 ] * cnt
	for i in range( cnt ):
		fir.add_sample( data[ i ] )
		fir_out[ i ] = fir.get_avg()

	fir_fft_out = np.fft.fft( fir_out ) / cnt
	freqs = np.arange( cnt ) / tmax
	fir_fft_out = fir_fft_out[ range( cnt // 2 ) ]
	freqs = freqs[ range( cnt // 2 ) ]

	plot_td_fd_cmp(
		ts, data, fir_out,
		freqs, fft_out, fir_fft_out,
		figname=figname, title=title
	)


MAF_TAPS = \
{
	"A": 128,
	"B": 128,
	"C": 2,
	"D": 96
}

IIR_A = \
{
	"A": 0.998,
	"B": 0.995,
	"C": 0.5,
	"D": 0.95
}


if __name__ == "__main__":

	from sys import argv

	tag = "A"
	if len( argv ) > 1:
		tag = argv[ 1 ]
	
	sig_name = f"sig{ tag }" 
	filename = f"{ sig_name }.csv"

	data = []
	cnt = 0
	tmax = 0.0
	with open( filename, mode="r" ) as f:
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

	figname = f"{ sig_name }.png"
	title = sig_name + f" (Fs={ int( sampling_rate ) }Hz)"
	plot_td_fd( ts, data, freqs, fft_out, figname=figname, title=title )

	maf_tap = MAF_TAPS[ tag ]
	maf_figname = f"{ sig_name }_maf.png"
	maf_title=f"{ sig_name } MAF ( tap={ maf_tap } )"
	maf_title
	run_maf( cnt, data, ts, fft_out, tap=maf_tap, figname=maf_figname, title=maf_title )

	iir_A = IIR_A[ tag ]
	iir_figname = f"{ sig_name }_iir.png"
	iir_title=f"{ sig_name } IIR ( A={ iir_A } )"
	run_iir( cnt, data, ts, fft_out, A=iir_A, figname=iir_figname, title=iir_title )

	fir_coefs = FIR_COEFS[ tag ]
	fir_figname = f"{ sig_name }_fir.png"
	fir_des = FIR_DES[ tag ]
	fir_title=f"{ sig_name } FIR ( { fir_des } )"
	run_fir( cnt, data, ts, fft_out, coefs=fir_coefs, figname=fir_figname, title=fir_title )

