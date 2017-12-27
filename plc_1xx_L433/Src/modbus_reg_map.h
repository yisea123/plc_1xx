#include "main.h"



//������ ��� �������� �������� 

//type: Swap Words
uint16_t settings[REG_COUNT];

uint16_t default_settings[REG_COUNT] = 
{		
		//����� ICP		
		0, 	//1. ���������� (���������) ������ ICP 
		0, 	//2. ---
0xC2C8,	//3. ������ ������� �����������������
		0,	//4. ---
0x4110,	//5. ������� ������� �����������������
		0,	//6. ---
0xC2C8,	//7. ������ ������� ���������
		0,	//8. ---
0x4120,	//9. ������� ������� ���������
		0,	//10. ---
		0,	//11. ���� ����������� ������ ICP (1 - ��������, 0 - �����)
0x3A03,	//12. ������� ��������� ������
0x126F,	//13. 
		0,	//14. ---
		0,	//15. 
0x3F80,	//16.	����. �������� 
		0,	//17.	---
		0,	//18. ����. �������� 
		0,	//19. ---
		1,	//20. ����� ��������� �������
0x4396,	//21. ?*�������� ���������� ���.�������� (�������)
		0,	//22. ---
		0,	//23. ���������, ���. ������� ���.�������� (������������ ��������)
		0,	//24. ---
		0,	//25. ��������, ���. ������� ���.�������� (������������ ��������)
		0,	//26. ---
		0,	//27. �����������, ���. ������� ���.�������� (������������ ��������)
		0,	//28. ---
		0,	//29. ��������/��������� ����� ICP (��� ������ �� ��������)
		0,	//30. 
		0,	//31. 
		0,	//32. 
		0,	//33. 
		0,	//34. 
		0,	//35. 
		0,	//36. 
				
	
		//����� 4-20
		0,	//37. ��� 
		0,	//38. ---
0xC2C8,	//39. ������ ������� �����������������
		0,	//40. ---
0x4190,	//41. ������� ������� �����������������
		0,	//42. ---
0xC2C8,	//43.	������ ������� ���������
		0,	//44. ---
0x41A0,	//45. ������� ������� ���������
		0,	//46. ---
		0,	//47. ���� ����������� ������ 4-20 (1 - ��������, 0 - �����)
0x4076,	//48. ������� ��������� ������
0x6666,	//49. ---
		0,	//50. 
		0,	//51. 
0x3BE5,	//52.	?*����. �������� 
0x6042,	//53.	---
		0,	//54. ?*����. ��������
		0,	//55. ---
0x4580,	//56. �������� �������
		0,	//57. ---
		0,	//58. ��������/��������� ����� 4-20 (��� ������ �� ��������)
		0,	//59. ��� 4-20
		0,	//60. ---
		0,	//61. ���-��� 4-20
		0,	//62. ---
		0,	//63. 
		0,	//64. 
		
		//����� 485 (modbus master)
		1,	//65.	����� �������������
0x4761,	//66.	�������� ������
		0,	//67.	---
 1000,	//68. �������
	998,	//69. ����� ������� �������� 
		0,	//70. ---
	0x3,	//71. �������
	 60,	//72. ���������� ���������
		1,	//73. ��������/��������� ����� 485 (��� ������ �� ��������)
		0,	//74. ���� ����������� ������ 485 (1 - ��������, 0 - �����)
		0,	//75. 
		0,	//76. 
		0,	//77. 
		0,	//78. 
		0,	//79. 
		0,	//80. 
		0,	//81. 
		0,	//82. 
		
		//����
		0,	//83. ��������� �������. ���� 
		0,	//84. ��������� ����. ����
		0,	//85. ����� ������ (0 - ��� ������, 1 - � �������)
		0,	//86. 
	100,	//87. �������� �� ������������, ��
		0,	//88. �������� ������� (�������� ������������)
	100,	//89. �������� �� ����� �� ������������, ��
		
		//����� 4-20
		0,	//90. �������� ������� ��� ������ (0 - icp, 1 - 4-20, 2 - 485)
		0,	//91. ����. ��������
		0,	//92. 
		0,	//93. ����. ��������
		0,	//94. 
0x41F0,	//95. �������� ��������
		0,	//96. 
		
		//���������� ����
		0,	//97. ������� ��������� (������ 1 � ������� - ������� ������������ ����)
		0,	//98.
				
		//����� ��������� 
		0,	//99.	���������� ������� ����������� 
		0,	//100. ---
		10,	//101. ����� ���������� (modbus slave)
0x47E1,	//102. �������� ������
		0,	//103. ---
		0,	//104. �������� CPU
		0,	//105. ---
		0,	//106. ������ ��
		0,	//107. ---		
		0,	//108. ���������� � ������������
		0,	//109. ����� �� �������� �� ��������� 
0x3E8,	//110. ����� ��������		
0x41B0,	//111. ������ ������� ������� �����������
		0,	//112. ---
0x41D0,	//113. ������� ������� ������� �����������
		0,	//114. ---		
		
		//����� 485 (modbus master) �����������
		2,	//115. ����� �������� 1
		0,	//116. ---
		0,	//117. �������� �������� 1
		0,	//118. ---
0x40C0,	//119. ����������������� ������� 1
		0,	//120. ---
0x40E0,	//121. ��������� ������� 1
		0,	//122. ---
		0,	//123. 
		0,	//124. 
		0,	//125. ����������� (ADXL � �������)
		0,	//126. ---
		55,	//127. ����� �������� 2
		0,	//128. ---
		0,	//129. �������� �������� 2
		0,	//130. ---
		0,	//131. ������ ����. ������� 2 
		0,	//132. ---
0x41F0,	//133. ������� ����. ������� 2 (����������������� �������)
		0,	//134. ---
		0,	//135. ������ ����. ������� 2
		0,	//136. ---
0x4270,	//137. ������� ����. ������� 2 (��������� �������)
		0,	//138. ---
		56,	//139. ����� �������� 3
		0,	//140. ---
		0,	//141. �������� �������� 3
		0,	//142. ---
		0,	//143. ������ ����. ������� 3 
		0,	//144. ---
0x42C8,	//145. ������� ����. ������� 3 (����������������� �������)
		0,	//146. ---
    0,	//147. ������ ����. ������� 3		
		0,	//148. ---
0x42C8,	//149. ������� ����. ������� 3 (��������� �������)
		0,	//150. ---
		57,	//151. ����� �������� 4
		0,	//152. ---
		0,	//153. �������� �������� 4
		0,	//154. ---
		0,	//155. ������ ����. ������� 4 
		0,	//156. ---
0x41F0,	//157. ������� ����. ������� 4 (����������������� �������)
		0,	//158. ---
		0,	//159. ������ ����. ������� 4		
		0,	//160. ---
0x4270,	//161. ������� ����. ������� 4 (��������� �������)
		0,	//162. ---		
		//����� ICP
		0,	//163. ��������� ��������� ICP
		0,	//164. ---
		0,	//165. ��������� �������� ICP
		0,	//166. ---
		0,	//167. ��������� ����������� ICP
		0,	//168. ---
		0,	//169. ������ ��������� ICP		
		0,	//170. ---		
		0,	//171. ������ �������� ICP		
		0,	//172. ---
		0,	//173. ������ ����������� ICP		
		0,	//174. ---
		//����� 485
		0,	//175. ���������� ������ ����� (�����)
		0,	//176. ������� ����� ������
		0,	//177. ������� ������ CRC
		0,	//178. ������� ������ TimeOut
		0,	//179. ���������� ���������� ������� 
		0,	//180. ---
		//����
	  0,	//181. ���������� ������ ����� (�����)
		0,	//182. ������� ����� ������
		0,	//183. ������� ������ CRC
		0,	//184. ������� ������ TimeOut
		0,	//185. ���������� ���������� ������� 
		0,	//186. ---
		0,	//187. ����� ����������
		0,	//188. ���./����. 


};

