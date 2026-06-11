
import csv
import matplotlib.pyplot as plt
import numpy as np

import serial

SER_PORT = "COM3"
SER_BAUD = 115200

try:
	SER = serial.Serial( SER_PORT, SER_BAUD, timeout=2 )
except:
	print( "failed to access serial port " + SER_PORT )
	exit( 2 )

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


def collect( sample_cnt ):
	import serial

	data = []
	print( "collecting force sensor data" )

	for i in range( sample_cnt ):
		try:
			line = SER.readline().decode( errors="ignore" ).strip()
			if 0 == i % 16:
				print( i, line )
			fields = line.split( "," )
			data.append( [ int( field ) for field in fields ] )
		except Exception as e:
			print( i, ": error ", e.__str__() )
			return data

	return data


if __name__ == "__main__":

	from sys import argv

	cnt = 1000
	if len( argv ) > 1:
		cnt = int( argv[ 1 ] )
	
	sig_name = "HX711_output" 
	filename = f"{ sig_name }.csv"

	data = collect( cnt )
	cnt = len( data )
	hald_cnt = cnt // 2
	tstart, tend = data[ 0 ][ 2 ], data[ -1 ][ 2 ] # ms
	total_dt = tend - tstart

	sampling_rate = cnt / ( ( total_dt ) / 1000.0 )
	print( f"Sampling rate: { sampling_rate } Hz" )
	ts = np.arange( cnt )
	freqs = ts / total_dt
	#freqs = freqs[ range( ha;l vfknw vlkhwe ljved_cnt ) ]
	freqs = freqs[ range( hald_cnt ) ]

	data = np.array( data )
	base, iir_out = data[ :, 0 ], data[ :, 1 ]

	fft_out = np.fft.fft( base ) / cnt
	fft_out = fft_out[ range( hald_cnt ) ]

	iir_fft_out = np.fft.fft( iir_out ) / cnt
	iir_fft_out = iir_fft_out[ range( hald_cnt ) ]

	#plot_loglog( freqs, fft_out )

	figname = f"{ sig_name }.png"
	title = sig_name + f" (Fs={ int( sampling_rate ) }Hz)"
	plot_td_fd_cmp(
		ts, base, iir_out, freqs,
		fft_out, iir_fft_out,
		figname=figname, title=title
	)
