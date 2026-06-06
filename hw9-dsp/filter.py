
from collections import deque

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

	def __init__( self, tap=10 ):
		self.tap = max( 1, int( tap ) )
		# pad buffer with null samples
		self.buf = deque( [ 0.0 for _ in range( self.tap ) ] )
		self.sum = 0.0
		self.avg = 0.0

	def add_sample( self, sample ):
		sample = float( sample )
		oldest = self.buf.popleft()
		self.buf.append( sample )
		self.sum = self.sum - oldest + sample

	def add_samples( self, sample_lst ):
		for smp in sample_lst:
			self.add_sample( smp )

	def get_avg( self ):
		return sum( self.buf ) / self.tap

"""
Infinite impulse response filter
"""
class IIR:

	def __init__( self, a=0.5 ):
		self.a = clamp( a, 0.0, 1.0 )
		self.b = 1.0 - self.a
		self.avg = 0.0

	def add_sample( self, sample ):
		sample = float( sample )
		self.avg = self.a * self.avg + self.b * sample

	def add_samples( self, sample_lst ):
		for smp in sample_lst:
			self.add_sample( sample )

	def get_avg( self ):
		return self.avg

"""
Finite impulse response filter
"""
class FIR:
	pass

