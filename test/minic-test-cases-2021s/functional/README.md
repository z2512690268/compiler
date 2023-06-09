# 测试用例说明

测试用例包括如下分类:

<table>
  <thead>
    <tr>
      <th>编号</th>
      <th>名称</th>
      <th>说明</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>1</td>
      <td>000_main.c</td>
      <td>简单main测试</td>
    </tr>
    <tr>
      <td>2</td>
      <td>001_var_defn.c</td>
      <td>简单全局变量定义</td>
    </tr>
    <tr>
      <td>3</td>
      <td>002_var_defn2.c</td>
      <td>简单局部变量定义</td>
    </tr>
    <tr>
      <td>4</td>
      <td>003_var_defn3.c</td>
      <td>简单全局与局部变量定义</td>
    </tr>
    <tr>
      <td>5</td>
      <td>004_const_var_defn.c</td>
      <td rowspan=2>简单const定义</td>
    </tr>
    <tr>
      <td>6</td>
      <td>005_const_var_defn2.c</td>
    </tr>
    <tr>
      <td>7</td>
      <td>006_break.c</td>
      <td>break测试</td>
    </tr>
    <tr>
      <td>8</td>
      <td>007_continue.c</td>
      <td>continue测试</td>
    </tr>
    <tr>
      <td>9</td>
      <td>008_add.c</td>
      <td rowspan=10>简单四则运算</td>
    </tr>
    <tr>
      <td>10</td>
      <td>009_add2.c</td>
    </tr>
    <tr>
      <td>11</td>
      <td>010_addc.c</td>
    </tr>
    <tr>
      <td>12</td>
      <td>011_sub.c</td>
    </tr>
    <tr>
      <td>13</td>
      <td>012_sub2.c</td>
    </tr>
    <tr>
      <td>14</td>
      <td>013_subc.c</td>
    </tr>
    <tr>
      <td>15</td>
      <td>014_mul.c</td>
    </tr>
    <tr>
      <td>16</td>
      <td>015_mulc.c</td>
    </tr>
    <tr>
      <td>17</td>
      <td>016_div.c</td>
    </tr>
    <tr>
      <td>18</td>
      <td>017_divc.c</td>
    </tr>
    <tr>
      <td>19</td>
      <td>018_rem.c</td>
      <td>取余运算</td>
    </tr>
    <tr>
      <td>20</td>
      <td>019_mod.c</td>
      <td>取模运算</td>
    </tr>
    <tr>
      <td>21</td>
      <td>020_arr_defn.c</td>
      <td>简单数组定义</td>
    </tr>
    <tr>
      <td>22</td>
      <td>021_arr_expr_len.c</td>
      <td>数组定义</td>
    </tr>
    <tr>
      <td>23</td>
      <td>022_array_traverse.c</td>
      <td rowspan=3>数组遍历</td>
    </tr>
    <tr>
      <td>24</td>
      <td>023_array_traverse2.c</td>
    </tr>
    <tr>
      <td>25</td>
      <td>024_array_traverse3.c</td>
    </tr>
    <tr>
      <td>26</td>
      <td>025_op_priority1.c</td>
      <td rowspan=5>运算符优先级测试</td>
    </tr>
    <tr>
      <td>27</td>
      <td>026_op_priority2.c</td>
    </tr>
    <tr>
      <td>28</td>
      <td>027_op_priority3.c</td>
    </tr>
    <tr>
      <td>29</td>
      <td>028_op_priority4.c</td>
    </tr>
    <tr>
      <td>30</td>
      <td>029_unary_op.c</td>
    </tr>
    <tr>
      <td>31</td>
      <td>030_if.c</td>
      <td rowspan=4>多重if嵌套测试</td>
    </tr>
    <tr>
      <td>32</td>
      <td>031_if2.c</td>
    </tr>
    <tr>
      <td>33</td>
      <td>032_if3.c</td>
    </tr>
    <tr>
      <td>34</td>
      <td>033_if4.c</td>
    </tr>
    <tr>
      <td>35</td>
      <td>034_while1.c</td>
      <td rowspan=2>多重嵌套while测试</td>
    </tr>
    <tr>
      <td>36</td>
      <td>035_while2.c</td>
    </tr>
    <tr>
      <td>37</td>
      <td>036_while_if_test1.c</td>
      <td rowspan=3>while-if混合测试</td>
    </tr>
    <tr>
      <td>38</td>
      <td>037_while_if_test2.c</td>
    </tr>
    <tr>
      <td>39</td>
      <td>038_while_if_test3.c</td>
    </tr>
    <tr>
      <td>40</td>
      <td>039_comment1.c</td>
      <td rowspan=2>简单注释测试</td>
    </tr>
    <tr>
      <td>41</td>
      <td>040_comment2.c</td>
    </tr>
    <tr>
      <td>42</td>
      <td>041_hex_defn.c</td>
      <td>十六进制测试</td>
    </tr>
    <tr>
      <td>43</td>
      <td>042_hex_oct_add.c</td>
      <td>十六进制与八进制加法</td>
    </tr>
    <tr>
      <td>44</td>
      <td>043_le.c</td>
      <td>小于等于测试</td>
    </tr>
    <tr>
      <td>45</td>
      <td>044_ge.c</td>
      <td>大于等于测试</td>
    </tr>
    <tr>
      <td>46</td>
      <td>045_not.c</td>
      <td>非测试</td>
    </tr>
    <tr>
      <td>47</td>
      <td>046_or.c</td>
      <td>或测试</td>
    </tr>
    <tr>
      <td>48</td>
      <td>047_and.c</td>
      <td>与测试</td>
    </tr>
    <tr>
      <td>49</td>
      <td>048_short_circuit.c</td>
      <td>短路求值测试</td>
    </tr>
    <tr>
      <td>50</td>
      <td>049_getint.c</td>
      <td>输入测试</td>
    </tr>
    <tr>
      <td>51</td>
      <td>050_void_function.c</td>
      <td>函数返回值为空</td>
    </tr>
    <tr>
      <td>52</td>
      <td>051_int_function.c</td>
      <td>函数返回值为int</td>
    </tr>
    <tr>
      <td>53</td>
      <td>052_scope.c</td>
      <td>作用域测试</td>
    </tr>
    <tr>
      <td>54</td>
      <td>053_prim.c</td>
      <td rowspan=41>简单算法小程序</td>
    </tr>
    <tr>
      <td>55</td>
      <td>054_fib.c</td>
    </tr>
    <tr>
      <td>56</td>
      <td>055_sum.c</td>
    </tr>
    <tr>
      <td>57</td>
      <td>056_factorial.c</td>
    </tr>
    <tr>
      <td>58</td>
      <td>057_water_flower_num.c</td>
    </tr>
    <tr>
      <td>59</td>
      <td>058_sort_test1.c</td>
    </tr>
    <tr>
      <td>60</td>
      <td>059_sort_test2.c</td>
    </tr>
    <tr>
      <td>61</td>
      <td>060_sort_test3.c</td>
    </tr>
    <tr>
      <td>62</td>
      <td>061_sort_test4.c</td>
    </tr>
    <tr>
      <td>63</td>
      <td>062_sort_test5.c</td>
    </tr>
    <tr>
      <td>64</td>
      <td>063_sort_test6.c</td>
    </tr>
    <tr>
      <td>65</td>
      <td>064_sort_test7.c</td>
    </tr>
    <tr>
      <td>66</td>
      <td>065_genealogical_tree.c</td>
    </tr>
    <tr>
      <td>67</td>
      <td>066_greatest_common_divisor.c</td>
    </tr>
    <tr>
      <td>68</td>
      <td>067_hanoi.c</td>
    </tr>
    <tr>
      <td>69</td>
      <td>068_monkey_peach.c</td>
    </tr>
    <tr>
      <td>70</td>
      <td>069_multiplication_puzzle.c</td>
    </tr>
    <tr>
      <td>71</td>
      <td>070_palindrome.c</td>
    </tr>
    <tr>
      <td>72</td>
      <td>071_reverse_order.c</td>
    </tr>
    <tr>
      <td>73</td>
      <td>072_exchange_array.c</td>
    </tr>
    <tr>
      <td>74</td>
      <td>073_is_leap_year.c</td>
    </tr>
    <tr>
      <td>75</td>
      <td>074_monkey_eat_peach.c</td>
    </tr>
    <tr>
      <td>76</td>
      <td>075_reverse_number.c</td>
    </tr>
    <tr>
      <td>77</td>
      <td>076_apple_and_norm.c</td>
    </tr>
    <tr>
      <td>78</td>
      <td>077_arithmetic_sequence.c</td>
    </tr>
    <tr>
      <td>79</td>
      <td>078_percolation.c</td>
    </tr>
    <tr>
      <td>80</td>
      <td>079_serpentine_filling.c</td>
    </tr>
    <tr>
      <td>81</td>
      <td>080_unlucky_data.c</td>
    </tr>
    <tr>
      <td>82</td>
      <td>081_change.c</td>
    </tr>
    <tr>
      <td>83</td>
      <td>082_complete_number.c</td>
    </tr>
    <tr>
      <td>84</td>
      <td>083_matrix_add.c</td>
    </tr>
    <tr>
      <td>85</td>
      <td>084_matrix_sub.c</td>
    </tr>
    <tr>
      <td>86</td>
      <td>085_matrix_mul.c</td>
    </tr>
    <tr>
      <td>87</td>
      <td>086_matrix_tran.c</td>
    </tr>
    <tr>
      <td>88</td>
      <td>087_cal_aaaa.c</td>
    </tr>
    <tr>
      <td>89</td>
      <td>088_is_arithmetic.c</td>
    </tr>
    <tr>
      <td>90</td>
      <td>089_least_common_multiple.c</td>
    </tr>
    <tr>
      <td>91</td>
      <td>090_2array_sum.c</td>
    </tr>
    <tr>
      <td>92</td>
      <td>091_backpack.c</td>
    </tr>
    <tr>
      <td>93</td>
      <td>092_multiplication.c</td>
    </tr>
    <tr>
      <td>94</td>
      <td>093_daffodils.c</td>
    </tr>
    <tr>
      <td>95</td>
      <td>094_long_array.c</td>
      <td>长数组</td>
    </tr>
    <tr>
      <td>96</td>
      <td>095_long_code.c</td>
      <td>长代码</td>
    </tr>
    <tr>
      <td>97</td>
      <td>096_many_param_call.c</td>
      <td>包含大量函数参数</td>
    </tr>
    <tr>
      <td>98</td>
      <td>097_many_global_var.c</td>
      <td>包含大量全局变量</td>
    </tr>
    <tr>
      <td>99</td>
      <td>098_many_local_var.c</td>
      <td>包含大量局部变量</td>
    </tr>
    <tr>
      <td>100</td>
      <td>099_register_realloc.c</td>
      <td>寄存器重分配</td>
    </tr>
    <tr>
      <td>101</td>
      <td>100_op_priority4.c</td>
      <td rowspan=12>保留测试</td>
    </tr>
    <tr>
      <td>102</td>
      <td>101_recursion_test1.c</td>
    </tr>
    <tr>
      <td>103</td>
      <td>102_recursion_test2.c</td>
    </tr>
    <tr>
      <td>104</td>
      <td>103_is_2power.c</td>
    </tr>
    <tr>
      <td>105</td>
      <td>104_recursion_test3.c</td>
    </tr>
    <tr>
      <td>106</td>
      <td>105_is_geometric.c</td>
    </tr>
    <tr>
      <td>107</td>
      <td>106_3digits.c</td>
    </tr>
    <tr>
      <td>108</td>
      <td>107_is_ugly_number.c</td>
    </tr>
    <tr>
      <td>109</td>
      <td>108_diagonal_sum.c</td>
    </tr>
    <tr>
      <td>110</td>
      <td>109_logi_assign.c</td>
    </tr>
    <tr>
      <td>111</td>
      <td>110_reverse_pairs.c</td>
    </tr>
    <tr>
      <td>112</td>
      <td>111_apple_and_norm2.c</td>
    </tr>
  </tbody>
</table>
