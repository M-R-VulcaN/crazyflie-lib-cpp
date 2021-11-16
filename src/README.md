# Crazyflie C++ Link - "src" directory

contains all files that are connected to the library

## Files Explained


### `ConnectionWrapper.cpp`

**`isBigEndian`** - returns true if the current cpu is big edian and false if it is small edian.

**`ConnectionWrapper`** - Takes a connection as an arguement, the class acts as a 'wrapper' to that connection given to the constructor, which it keeps an reference of.

**`recvFilteredData`** - returns the first Packet recieved with the specified port and channel. If no port and channel specified it uses the same port and channel used by the ConnectionWrapper itself.

**`sendData`** - takes 2 or 4 arguements. The pointer of the data and the second argument is the size of the data. It combins the first and second data together (if porvided) and sends it all to the crazyflie. Sends the data through the connection by writing it into the packet

**`getConnection`** - returns a reference to the connection stored in the connectionWrapper.

**`setPort`** - sets the port of the packet used by the connectionWrapper.

**`setChannel`** - sets the channel of the packet used by the connectionWrapper.




# METHODS and ARCHITECTURE

Note: The difference between V2 and V1 is the use of 2 bytes instead of 1 to represent the ID of the param, increasing the potential size of the TOC by 256 times.


## Set Param Protocol Structure

**<span style="text-decoration:underline;">Set Param Value:</span>**

*Send:*


<table>
  <tr>
   <td>Byte #
   </td>
   <td>2
   </td>
   <td>sizeof(&lt;param type>)
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>&lt;param id></strong>
   </td>
   <td><strong>&lt;param value></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Param ID
   </td>
   <td>The value of the param
   </td>
  </tr>
</table>


*Receive:*


<table>
  <tr>
   <td>Byte #
   </td>
   <td>2
   </td>
   <td>sizeof(&lt;param type>)
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>&lt;param id></strong>
   </td>
   <td><strong>&lt;new param value></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Param ID
   </td>
   <td>The new value of the param
   </td>
  </tr>
</table>



## Read Param Protocol Structure

**<span style="text-decoration:underline;">Read Param Value:</span>**

*Send:*


<table>
  <tr>
   <td>Byte #
   </td>
   <td>2
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>&lt;param id></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Param ID
   </td>
  </tr>
</table>


*Receive:*


<table>
  <tr>
   <td>Byte #
   </td>
   <td>2
   </td>
   <td>sizeof(&lt;param type>)
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>&lt;param id></strong>
   </td>
   <td><strong>&lt;param value></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Param ID
   </td>
   <td>The value of the param
   </td>
  </tr>
</table>



## TOC Protocol Structure

**<span style="text-decoration:underline;">Get Toc Item V2:</span>**

*Send:*


<table>
  <tr>
   <td>Byte #
   </td>
   <td>1
   </td>
   <td>2
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>2</strong>
   </td>
   <td><strong>&lt;param id></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Command  Number
   </td>
   <td>Param ID
   </td>
  </tr>
</table>


*Receive:*


<table>
  <tr>
   <td>Byte #
   </td>
   <td>1
   </td>
   <td>2
   </td>
   <td>1
   </td>
   <td>strlen
   </td>
   <td>strlen
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>2</strong>
   </td>
   <td><strong>&lt;param id></strong>
   </td>
   <td><strong>&lt;type code></strong>
   </td>
   <td><strong>&lt;group name string></strong>
   </td>
   <td><strong>&lt;param name string></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Command #
   </td>
   <td>Param ID
   </td>
   <td>Param Type
   </td>
   <td>Group Name
   </td>
   <td>Param Name
   </td>
  </tr>
</table>


**<span style="text-decoration:underline;">Get Toc Info V2:</span>**

*Send:*


<table>
  <tr>
   <td>Byte #
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>3</strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Command  Number
   </td>
  </tr>
</table>


*Receive:*


<table>
  <tr>
   <td>Byte #
   </td>
   <td>1
   </td>
   <td>2
   </td>
   <td>4
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>3</strong>
   </td>
   <td><strong>&lt;size></strong>
   </td>
   <td><strong>&lt;crc></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Command #
   </td>
   <td>Number of elements in the Toc
   </td>
   <td>Current <a href="https://en.wikipedia.org/wiki/Cyclic_redundancy_check">Cyclic Redundancy Check</a> value
   </td>
  </tr>
</table>


## **LOG PROTOCOLS** (Port = 5)

Note: The difference between V2 and V1 is the use of 2 bytes instead of 1 to represent the ID of the param, increasing the potential size of the TOC by 256 times.


### Log Settings Protocol Structure (Channel = 1)

**<span style="text-decoration:underline;">Create Block V1:</span>**

*Send:*


<table>
  <tr>
   <td>Byte #
   </td>
   <td>1
   </td>
   <td>1
   </td>
   <td>1
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>0</strong>
   </td>
   <td><strong>&lt;new block id></strong>
   </td>
   <td><strong>&lt;first log type></strong>
   </td>
   <td><strong>&lt;first log id></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Create Block V1 
<p>
Command
   </td>
   <td>The ID of the new log block
   </td>
   <td>The new block’s first log variable’s type
   </td>
   <td>The new block’s first log variable’s ID
   </td>
  </tr>
</table>


*Receive:*


<table>
  <tr>
   <td>Byte #
   </td>
   <td>1
   </td>
   <td>1
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>0</strong>
   </td>
   <td><strong>&lt;new block id></strong>
   </td>
   <td><strong>&lt;result status></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Create Block V1 
<p>
Command
   </td>
   <td>The ID of the log block you have requested to create
   </td>
   <td>Returns a success status [0] or the <a href="https://github.com/torvalds/linux/blob/master/include/uapi/asm-generic/errno-base.h">error</a> it encountered
   </td>
  </tr>
</table>


**<span style="text-decoration:underline;">Create Block V2:</span>**

*Send:*


<table>
  <tr>
   <td>Byte #
   </td>
   <td>1
   </td>
   <td>1
   </td>
   <td>1
   </td>
   <td>2
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>6</strong>
   </td>
   <td><strong>&lt;new block id></strong>
   </td>
   <td><strong>&lt;first log type></strong>
   </td>
   <td><strong>&lt;first log id></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Create Block V2 
<p>
Command
   </td>
   <td>The ID of the new log block
   </td>
   <td>The new block’s first log variable’s type
   </td>
   <td>The new block’s first log variable’s ID
   </td>
  </tr>
</table>


*Receive:*


<table>
  <tr>
   <td>Byte #
   </td>
   <td>1
   </td>
   <td>1
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>6</strong>
   </td>
   <td><strong>&lt;new block id></strong>
   </td>
   <td><strong>&lt;result status></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Create Block V2 
<p>
Command
   </td>
   <td>The ID of the log block you have requested to create
   </td>
   <td>Returns a success status [0] or the <a href="https://github.com/torvalds/linux/blob/master/include/uapi/asm-generic/errno-base.h">error</a> it encountered
   </td>
  </tr>
</table>


**<span style="text-decoration:underline;">Append to Block V1:</span>**

*Send:*

<table>
  <tr>
   <td>Byte #
   </td>
   <td>1
   </td>
   <td>1
   </td>
   <td>1
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>1</strong>
   </td>
   <td><strong>&lt;block id></strong>
   </td>
   <td><strong>&lt;log type></strong>
   </td>
   <td><strong>&lt;log id></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Append to Block V1 
<p>
Command
   </td>
   <td>The ID of the log block which you want to append
   </td>
   <td>The type of the log variable which you wish to append
   </td>
   <td>The ID of the log variable which you wish to append
   </td>
  </tr>
</table>


*Receive:*


<table>
  <tr>
   <td>Byte #
   </td>
   <td>1
   </td>
   <td>1
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>1</strong>
   </td>
   <td><strong>&lt;block id></strong>
   </td>
   <td><strong>&lt;result status></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Append to Block V1 
<p>
Command
   </td>
   <td>The ID of the log block which you have requested to appended
   </td>
   <td>Returns a success status [0] or the <a href="https://github.com/torvalds/linux/blob/master/include/uapi/asm-generic/errno-base.h">error</a> it encountered
   </td>
  </tr>
</table>


**<span style="text-decoration:underline;">Append to Block V2:</span>**

*Send:*


<table>
  <tr>
   <td>Byte #
   </td>
   <td>1
   </td>
   <td>1
   </td>
   <td>1
   </td>
   <td>2
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>7</strong>
   </td>
   <td><strong>&lt;block id></strong>
   </td>
   <td><strong>&lt;log type></strong>
   </td>
   <td><strong>&lt;log id></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Append to Block V2 
<p>
Command
   </td>
   <td>The ID of the log block which you want to append
   </td>
   <td>The type of the log variable which you wish to append
   </td>
   <td>The ID of the log variable which you wish to append
   </td>
  </tr>
</table>


*Receive:*


<table>
  <tr>
   <td>Byte #
   </td>
   <td>1
   </td>
   <td>1
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>7</strong>
   </td>
   <td><strong>&lt;block id></strong>
   </td>
   <td><strong>&lt;result status></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Append to Block V2 
<p>
Command
   </td>
   <td>The ID of the log block which you have requested to appended
   </td>
   <td>Returns a success status [0] or the <a href="https://github.com/torvalds/linux/blob/master/include/uapi/asm-generic/errno-base.h">error</a> it encountered
   </td>
  </tr>
</table>


**<span style="text-decoration:underline;">Delete Block:</span>**

*Send:*


<table>
  <tr>
   <td>Byte #
   </td>
   <td>1
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>2</strong>
   </td>
   <td><strong>&lt;block id></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Delete Block 
<p>
Command
   </td>
   <td>The ID of the log block which you want to delete
   </td>
  </tr>
</table>


*Receive:*


<table>
  <tr>
   <td>Byte #
   </td>
   <td>1
   </td>
   <td>1
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>2</strong>
   </td>
   <td><strong>&lt;block id></strong>
   </td>
   <td><strong>&lt;result status></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Delete Block Command
   </td>
   <td>The ID of the log block which you have requested to delete
   </td>
   <td>Returns a success status [0] or the <a href="https://github.com/torvalds/linux/blob/master/include/uapi/asm-generic/errno-base.h">error</a> it encountered
   </td>
  </tr>
</table>


**<span style="text-decoration:underline;">Start Receiving from  Block:</span>**

*Send:*


<table>
  <tr>
   <td>Byte #
   </td>
   <td>1
   </td>
   <td>1
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>3</strong>
   </td>
   <td><strong>&lt;block id></strong>
   </td>
   <td><strong>&lt;period></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Start Block Command
   </td>
   <td>The ID of the log block which you want to start to receive from 
   </td>
   <td>The rate in centi-seconds by which to send the log
   </td>
  </tr>
</table>


*Receive:*


<table>
  <tr>
   <td>Byte #
   </td>
   <td>1
   </td>
   <td>1
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>3</strong>
   </td>
   <td><strong>&lt;block id></strong>
   </td>
   <td><strong>&lt;result status></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Start Block Command
   </td>
   <td>The ID of the log block which you have requested to delete
   </td>
   <td>Returns a success status [0] or the <a href="https://github.com/torvalds/linux/blob/master/include/uapi/asm-generic/errno-base.h">error</a> it encountered
   </td>
  </tr>
</table>

**<span style="text-decoration:underline;">Reset Blocks:</span>**

*Send:*

<table>
  <tr>
   <td>Byte #
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>5</strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Reset Blocks Command
   </td>
  </tr>
</table>

*Receive:*

<table>
  <tr>
   <td>Byte #
   </td>
   <td>1
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td>Value:
   </td>
   <td><strong>5</strong>
   </td>
   <td><strong>&lt;result status></strong>
   </td>
  </tr>
  <tr>
   <td>Description:
   </td>
   <td>Reset Blocks Command
   </td>
   <td>Returns a success status [0] or the <a href="https://github.com/torvalds/linux/blob/master/include/uapi/asm-generic/errno-base.h">error</a> it encountered
   </td>
  </tr>
</table>




 
 



