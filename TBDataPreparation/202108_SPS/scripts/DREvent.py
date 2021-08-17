# Conversion fron ascii data format to DREvent class

class DREvent:
  ''' Class that represent a Dual Readout event at TB 2021 @H8 '''
  
  def __init__(self):
    ''' Constructor '''
    self.EventNumber = 0
    self.NumOfPhysEv = 0
    self.NumOfPedeEv = 0
    self.NumOfSpilEv = 0
    self.TriggerMask = 0
    self.ADCs = {}        # Simple dict      key(channel) : value
    self.TDCs = {}        # Dict key:tuple   key(channel) : ( value, check )

  def headLine(self):
    """Write header in ascii data dump"""
    return "%8s, %8s, %8s, %8s, %6s, %6s, %6s" % (
           "evNum", "#phEv", "#peEv", "#spEv", "trigM",
           "#ADCs", "#TDCs", )
  
  def __str__(self):
    """Overload of str operator (for data dump)"""
    return "%8d, %8d, %8d, %8d, %6d, %6d, %6d" % (
          self.EventNumber, self.NumOfPhysEv, self.NumOfPedeEv, self.NumOfSpilEv, self.TriggerMask,
          len(self.ADCs), len(self.TDCs) )

  def getAdcChannel(self, ch):
    """get the data value for ADC channel ch"""
    return self.ADCs[ch]

  def getTdcChannel(self, ch):
    """get the data value for TDC channel ch"""
    return self.TDCs[ch]  # Tuple: (value, check)

# Parse the evLine and return a DREvent object
def DRdecode(evLine):
  """Function that converts a raw data record (event) from
     ascii to object oriented representation: DREvent class"""
  # Create new DREvent
  e = DREvent()

  # Split sections
  strHeader  = evLine.split(":")[0]
  strPayload = evLine.split(":")[1]
  strPayload = strPayload.replace("TDC", ":TDC")
  strADCs    = strPayload.split(":")[0]
  strTDCs    = strPayload.split(":")[1]
	
  # Parse strHeader
  hList = strHeader.split()
  e.EventNumber = int( hList[2] )
  e.NumOfPhysEv = int( hList[5] )
  e.NumOfPedeEv = int( hList[6] )
  e.NumOfSpilEv = int( hList[7] )
  try:
    e.TriggerMask = int( hList[10], 16 ) & 0xFF
  except ValueError:
    print 'ERROR: INVALID TRIGGER MASK:', hList[10] 
    e.TriggerMask = 0
   
  # Parse ADC 
  listADCs = strADCs.split()
  for i in range(len(listADCs)):
    if i%2 ==0:
      ch = int(listADCs[i]  , 10)
      val= int(listADCs[i+1], 16)
      e.ADCs[ch]=val 

  # Parse TDC  
  entries  = int(strTDCs.split()[2], 10)
  strTDCs  = strTDCs[ strTDCs.find("val.s") + 6 : ]
  listTDCs = strTDCs.split()
  if entries > 0:
    for i in range(len(listTDCs)):
      if i%3 ==0:
        ch  = int( listTDCs[i+0], 10 )  # Channel
        ver = int( listTDCs[i+1], 10 )  # Varification number
        val = int( listTDCs[i+2], 10 )  # Value
        e.TDCs[ch] = ( val, ver) 
 
  return e


