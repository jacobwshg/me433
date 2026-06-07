
from collections import deque
import numpy as np

def clamp( val, mn, mx ):
	assert( mn <= mx )
	if val > mx:
		val = mx
	if val < mn:
		val = mn
	return val

"""
Moving average filter
"""
class MAF:

	def __init__( self, tap=4 ):
		self.tap = max( 1, int( tap ) )
		# pad buffer with null samples
		self.buf = deque( [ 0.0 for _ in range( self.tap ) ] )
		self.sum = 0.0
		self.avg = 0.0
		self.valid = False

	def add_sample( self, sample ):
		sample = float( sample )
		oldest = self.buf.popleft()
		self.buf.append( sample )
		self.sum = self.sum - oldest + sample
		self.valid = False

	def add_samples( self, sample_lst ):
		for smp in sample_lst:
			self.add_sample( smp )

	def get_avg( self ):
		if not self.valid:
			self.avg = sum( self.buf ) / self.tap
			self.valid = True
		return self.avg

"""
Infinite impulse response filter
"""
class IIR:

	def __init__( self, A=0.5 ):
		self.A = clamp( A, 0.0, 1.0 )
		self.B = 1.0 - self.A
		self.avg = 0.0

	def add_sample( self, sample ):
		sample = float( sample )
		self.avg = self.A * self.avg + self.B * sample
		self.valid = False

	def add_samples( self, sample_lst ):
		for smp in sample_lst:
			self.add_sample( smp )

	def get_avg( self ):
		return self.avg

"""
Finite impulse response filter
"""
class FIR:

	def __init__( self, coefs ):
		self.tap = len( coefs )
		assert ( self.tap > 0 )

		self.coefs = np.array( coefs )
		self.buf = np.zeros( self.tap )
		self.avg = 0.0
		self.valid = False

	def add_samples( self, sample_lst ):
		sample_cnt = len( sample_lst )
		if sample_cnt >= self.tap:
			self.buf = np.array( sample_lst[ sample_cnt-self.tap:, ] )
		else:
			self.buf = np.concat(
				(
					self.buf[ sample_cnt:, ],
					np.array( sample_lst )
				)
			)
		self.valid = False

	def add_sample( self, sample ):
		sample_lst = [ sample ]
		self.add_samples( sample_lst )

	def get_avg( self ):
		if not self.valid:
			self.avg = self.buf @ self.coefs
			self.valid = True

		return self.avg

