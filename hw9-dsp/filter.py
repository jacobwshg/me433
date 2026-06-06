
from collections import deque

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


