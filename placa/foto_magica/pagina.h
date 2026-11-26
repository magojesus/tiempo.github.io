// Página de la app (Mind Search / Notas) y su manifest.
// Va en un archivo aparte para que el Arduino IDE no se confunda con el HTML.
#pragma once
#include <Arduino.h>

const char PAGINA[] PROGMEM = R"HTML(<!DOCTYPE html>
<html lang="es"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no,viewport-fit=cover">
<meta name="theme-color" content="#ffffff">
<meta name="apple-mobile-web-app-capable" content="yes">
<meta name="mobile-web-app-capable" content="yes">
<meta name="apple-mobile-web-app-status-bar-style" content="default">
<meta name="apple-mobile-web-app-title" content="Mind Search">
<link rel="apple-touch-icon" sizes="180x180" href="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAALQAAAC0CAMAAAAKE/YAAAABgFBMVEX5/v/x/f/t/P/r+v/p+v/q9/7k9P/h8//h8P/h8P7j7Pfd8P7G8f6b5/597v+z2vqtv/B24P5vvPiRnu10lPl3ee9sdOZZl/ZWfulfctRKcdJ8XuJoXthaZMNbV8BFYsJHVa8mWbQfUqppSMdYRrVfN7RVMqdPRKpQNaNQL6VPLp1FSZpEP5dFNqZDNpNJL6NDMaJBLp9FL4ktTK4lR6EdT6cfS6UwQKYzO6MnQaIuPJc3N6IwOJ83NKA6MZ81MZ05LZw4MXFSKqJPKp5QKJ5LKZ5KKJFNJppLI5pIJZNIIZVDKJw8KJpEJ4Y8J3dCIphDIoc/In06InRHIJVGHZVDH5BDHZBAHYY9Hn86H3Y5HXhEG5JBGo4/Gok6G30+GYg8GIQ6GH85F380Jmg0IGk2HW8yHWctIVssHlosHVsrHFg1GnM2F3cxGmgsG1snGlEmGVAkGEwgGEY0FXYnFlgeFUMbEj8VETgRDzMRDTIODTEODDEODDAOCiwJByYDACHLpLxkAAAcUElEQVR42r3cCVsUx9YA4J4VnIVuaHYdUEFkuUYjMcaA6IgMiyKLLCKrKJAIxgSusnzA/PXvnNqrunoWvE/OHZYA9/HlcKq6urqqnEuICxLnEKcQJxDHPL7T+CbiEOMfFn+T+MrjM4+/QuJLdbG/f6THAQ2HkyszHyrmv//WzZ/LmHd3d/83ZkDr5pNqzSb4858YzCk+B+8uiWrIYeYDp0SeS5H/CUkyNVMs/2RXxv8gzRKt5Ll0mpVq/mqpiz8tUZV5fx/eMMvh5u1tp0LzYWiabUm2eCU73L6vRHieGdpi/q6ZD4PFXKowGNhitqR8Z4e87e/s7OsRlmeKDi/nULNRGX/8UUGSbegdFvumeP8o3IzocuZAZXwtRw7HfsK3T1++fILc7qixXwJ9YJgBXa7bMM2B5lfe/OkTvtF3EDtm7FvM+2FpRnRVZkst/wHx554IW24/qbGzE0QHzftHB6i1mz84AbOtNA6taf6DiPf2LGTi2zW81ixbzZxpNwOamUul+dCsZp5i1czq4FOpsIEtaT6whTR/cE51dCWlIcwC/NGMCsWWNB/YzQqZoSsoZ6NrLinm7o/lxSsr+/v49v49eXv//uDgoKyZoO3mQOesVnM5sZZyAzo/P0/frYh4/56IMQ4QbuI189YWoO1XbmtpsP7CJs6r8ebN/Jt5IxBKsDRWVnSzEQcHG9S7ETQjuqz5b9UsyHYuNSMbYz48SqM3Ng7gBW/43jQDuhJzsDBCwSTPbxg6nL5SDk1iG156OW8RdCVN0CTbxA+NeKNHSfOK7uVk6v7wQU+zgg4z/62Yw8gE+ZMSdnlobQjzxnvNi4FU08zRYWPnoNkQI/c/lpByS8pXVqQbzCt6TQTQHwwzQ1vMgXJWzFJMwAPB0OEm+83K/MqczPXc3Pt3795vvMPY2Hhnknlw8uamE1IbZjkHyQb4pggVzt3T05p77g1IV9gbBgfTj9weYiboqswKOcBVQ8CFW4DfzJlBrQ8fMrXIeZCMZkRXalbS/JDnmBNzamhwyZ6enguJd6ieO8r5R3PvtKBeKaZmiVYK+h/TbKT5oSbO2aODfPNuZex3BO2lDnTzlublZESXNf9hkH9SyBTYBtFOoo2G4r5L3awjnLa7gQxvKUerj3dbZjDzuhMoDtX8OWB++FCSKY1Sm30Sjc1tbSq8g6VbzbbdPXvQ7TgdR7NcvPxueXkLXhYzRwfNX+1mWhmCDGDf99x0MhkhEa9Ju67nN0t3RwfNtppsG3v6yHccD9HL8BKhuDmZo+3mrxazSkaxm46CNRqNxmhE8T8jNa7XKNyUfVck266e3Ug5Tmp2dnY5EMDlWSZmijZnveTQWTOzauZkEEN+QUuccRLkU/KVWMZrUtigNpIt4SCdnj7qdSDuHgXYm8ubMtYt6LA8K6XByc1eGmQJUNdkMtlstp5EXTbrZtIx+E48Ekm6voWtqGfnpucQPLtxtPHQR7T/8N3Rh2XdvQ5WzbzmaGNR49JtmGmaGTnmRBPRSDyTrUMsqF0Miq/LZuBvkAB3hrPtavDOzm0fbfT6XsSh4fm9y0fbW4p5HV7rm+RFzYCuxszS3ObFIvFEJJpBIqQ27igRTdMvZ+Ik3W6jSHY3qWxaIlL9sMNPOVqk/Fy/gka3EmuANqZxlXtBco9iMbf7aQfI8Uwd0iBDKc/P5Wgnnsv5HhLS6HZrkJ3wmnmyuVrN9ezDNi/hmOre5Vk11xqZo4NzG9L8UTGTNLuRaDwSd6k44uXyhxdFJS6/5wcQjvnO1kCROOlGU63menb7SE12yu8obG9vyfa4HjBLdOAO1m4maY5FMgBKQwHm9i6LxYuzEyVO8Vc4HID0JeEXgyJJRhJ+c7NU96rXGer+sL3cSxti//L2NjTEWYG2mCk6zLxnMUcjiUhNFrLsJPz8ZfESwGfnWpzBnAT8Knu5lJPM1tdlIvGY45lqrRPBd9u0y9vGtjnL1WvLawAFLOdKtG3MH2L2WJozkJS9YvH05PTcGmfo/gZsKPxsFMop04SVzdW9PxmdCMLx4lKYlbG4tri4tra4FgxHN/MBx+dQczQGaY47Xr54eXJyXiJOTy6KX+Fv7mJlJyPpJpJsRV0oPCxIdGEbL+PbQr3IY21tSScvLTkB89dS5iSWRsZJDVwWw5KspvuymIdkwx8GaiqgLmDIXG91OM51hl5UY2lJdy+tOtbaUIqDXAelOY2l4e1BYZxXEKcnxWMfKzsTzHW/oZ4tpJy7y0EzoGlQ+tLSKkGXMud/Msx1NY5/clkRGQNqZMCJBNW9oC6wIOZCYdtLrRdmF03z4pLmXl0laLOD1npoURzcnHAGihcVm3HKHkuEqJ1MM6hzXN0v1eT9tg8lvWiNJRGrFB3s7JRLoSiOdh/LkppPT8+riZPiR6aO8J6v21CTWO7tXi9MLk6GoVepWKIDU4xmcbQlo7F4fT2azX65UnU6Eo/4vEC6SYHo6tnFSRaLGl3JMUMH7mL1VojFgWZs//Dvovm86kB1rK4+GYsmG1V1v6EuTMpYXJzi6Lc6maO1SUalOHiiSUFnoN9ou4r5/PwYWmNNfRZSTcr6hr1AJrWYmiLutxBLCwurC6v4IrHgmFcVsxWS4miG0VqmPut4xdOzq6Ah1znor+GKzi7o1gIxzBhvSSywWF1A+gJB/1020S6MMevrIqnDy9Pzq8XppUfKOlrbSFPNe5AQ9dSUol6QwdHapPmfe8FEQ88RJwWdDymOszMxzoNPQ9CHqWh9XTQRcUmB2FMt3VNTkwpac6voz2pvpyW6DRIEFen4FjNyzy4uLsVo+gIHqhb5CZQ1KZAopLr5hq0tTk7KXE9p9RFAa/X855/WRGPPEU99vzgLjOWQe3n8bY/P/u59P8avXAQGrKDGAonTVN9QUt1fsLGnxidD1Y7WCFVzXkk06TnM3u4MLtHF472BzpYGuJdlUV/f0H4zv3cCtwYG++Qyn0jXuzDXYKa631LV42ttHWsEHaxrjhbLTEIr2nEvzlXFyRkMmPOdDQybZcHoDTfz3/EGQU+1T1PtmakeD7bE8VWvbW2ClciUBa0kGtC7Jhq6jpp6V0/0yXnxMt9ZT7mZdDp9rTYJUXsNPs0Qen1DJ9zYnJ+obXEvgVUdTTc2AVrpQPrHx3X0+Phqb8bfnJiYEJ2IYp5xlN5OTzRUB7uAx0lFH8uKPjspnuVbKBixtbXxeBK46SR8xC8weEv+TL3oY1XX1UED8QOpRvV4AT6Mg3hyanW138+43Qtrq6CeYGjSicDHGUCr5pBmGIeuIycTDa0vj2VBxLW16dp4retfJ+F7aZiDrMWvZ5DdkL+UQ8KTYh46EJj8c5uadDSqZUyM5vxsxoWX3z2B2Z6YeCva4wyEo5lZdeQlug1mDLAZRvb4dQXS/L0NPDhVWouRrPWuK+Gn4vTLybQLP9b2XUn2hQu/P15gmpRUB9Dj490wfZJ1/eZc7wSNt8xNzAStmXf16mhrTsagOiLeJfuXzy6K+Xq4W01TMVRG6roRXpx9KwlzNnX1+SIvLLyYZ+tisYgvU91jS/XC6mZHxltdW12YEEHQMxz92Ux0qeo4uThuQ3Kilpu964Hw+S9Um0B22zErEVYfMaU+uil6dNyMUddfHR2fUNBTM8ws0H+FVEe7F4mlZXWcXB431NW7AqWYoXE1s0+7pLoWZmzqGo75/dn/QX1g/9FE60Og+0dN9qqfW1XNE1MzIhiarbeU1SFKOoMlHXf5BWIPWmA6aG4WQdVe/JpQp6E97lE1dtVQ1LGkUtQs1SZ6Ide9oJonZhT0f/+rLF6EVUhGSbdBY4crCx12nFwe1mvmmpRphj+7XtdUXX9I1DgAge5TL+oeijbVExPjFaF3g2gfZvrrsvTKcnYBtZGtVSLuc3NrawtEYyqS8qn6mvpzWagQbI2nxY+0qD2lqHt6+yiasl+q7hLovyRaL2lshzCSdumg9KTYppuTKWFuaf9972vehem4RJOZaqJuo7/2txS0kChD06Lu6emjqX75EsTkHZOHo3FheaA6jHaY+gfaIbT9uvq0IrlGE03MeRyXXpCJZpbqpIqG+3g6GL/wsCXC1J4o6jsc/fIlvHiQnCsVMvNKRytmGzoKf08clUJxwBSoRrnGEk3MuLmAzDI30taYUn80CRORWCCnRS9S75poUJNEKzHO5Bz96pWidmSidwX6DUXn+PUw6ZLiuFmX1cxJlyW65SaQgV3cA7XXStFKB4I/m627Calm3QeOmZpU9IsXBprRx1Q0Zb+eee18/stEw0oBetOSoz2eC9dDuKHFRKc1NO3vsBHCMJTcthSP81/bGdrX0WmSanJNBHSyMvSYima5fv1aQdNllwSdl2iXo7Gi9USLkm5tFzdb8DbQwjpr7Ycx1VDV4egXL+xofDEz5PrVawyHb5LY5eh5lumbFnStjmaZbuks8ntEqJLf7ehaK7pLRQfdYzReydDQuxWh7ZlW0PBJVZnmaKJGt4Wsmi3oT1Z0tnxNn8hUFztbKq/pLobuYWjM9YuyZgsazRoahh7eha33iIve43eOviz+01K692hz6kqhFXdJ9J9KdQTQpJ/OpL7Z+umaNO+nWw9JNw0VfdnewkZNbpl+WkP3vNBCNQcqmqPl+nJaHbL3YOhD+AdLXRFBTeKinSdaH3zwK+LZZRB9J4DWzGMBcwn0XYKGsUcydOzBBqZEPfDP2cXh761oZiVtHXsc49gjgncBJTL9cnR0rESiKVpZyP9xfl5cXDooGm9cQkZ5STEwhUFeaysl00SnamyjPDKLQEd5JdCjKrp0psmK8nkD3ZQsNZ6mF3I6nAZzq7gN6KpgPB1eHqOjL0qZCVrdMbGDZgWNc2JufZTd11ruXLqsdy5+2J1LG/Z4sURjSTSwS5mfm2iWaT6exj4PW2LikKuNe8Sk36XevPAbxnSN9R6RtUM+XkL0HRtalgf1/gwvLn79HNG7VjTv89jd+EDo3bgfvBu/Lv4W+t04lDTOoUR5OwxDy5omYhqUTEKiPzH0G9l9YH00xsj0o8cnmILzHl5wBqGmxLwHlrRoh1Z0n5ron3m8/pmbAa2Zd3imBbqZ3I6nU4ditsacYYqnfNtcjWWGiVQHK2kLGi+LMArp6xu9P3YfX/fvSzPEcx7OronWMk0uL9BTZ9VJU3MuLxlPiWzDsrdk2Fweqw5IQmOjiWap7sMYvS/Can7ufNH6DgXNi7oxivURcS/U6XRj1jSJKx89z0vB9CnJccisKU5Qw6yYF4Ym5r77VvTz5/ZM020ogJ4O1EdGf0oUmJ9OJhOxWDyZLDE/jbfiNZDoWNKX5XFHoElhGOgHVrNA871KJpr2H5FsHT5DPP/BJwHYDGE+0220oPtk2Mw29KcS6OZr0WCqq3/mAkOTVJw8tfUtaKtZQ4+McPMzJdN8YxVH8/qAppiAJ9uOWtVXerqFFV0TjaQbS6GV2lDRI8L87BmgvyjoeVbTbx6qqcbxh+3xVnXPES8/0odb2AyDDTFo1tAjP3P0M0R/+SKboUSz+rjbwVKNHUgi9fXyyk9sz2BuiV5YMrA0XEffCUc/EHmm6GfPJJpvwNPQP/3nLk11c3MaH3DB8/yLs7MrPhvHoRJ5YBvzJJqp1YZomB9I88jzEWZG9BetokVRK6luxkeJP7Bygo5J06QVuj5DN1231IcQo/nBAw09MvLsmZrpXbm/kWZ6jjbFu3e7aapd9kh/4GrqE5gwi9ZBcURrPLJ1oFGOmLrV+lDzLMwWNClpw0zRWB8s1Y3XcDVQXSzx8Uoray6PXboeKArXTZ7qZlbV3TLVpvmBSjbQn1Q0qufmaH3IVGOBwN/XSV1BDWYPFkJmsMayHlMrqebTCEp1KOigmaFVs4JWUt1MFwRdRU3MGVLQaXig2aClWlH39fUaZDA/sJhpTe9o6HncWzWtpJqqXa7OV7de7KT4jZmjSVhxHVXV+DCgCx8H4POA3oAZwmYm6C8mmqhFqlmBNKUjRB2JVLOakK4lxP8bjKdc7PIiboPoQIiaoMlTrnv37t2/f6+smXR5Oyp6haKng6nm6jis26w02bBEdiACbRDMUWqOqepmqkY2rj+9h/Hg3j2L+emzp8I8bKJhUybdeTetppqqG9Mw3oM1umlcIVtJsmEA+42tj41TM6KpulFVk1TfvccjYH4KZoEeVtFs8+s825ZkV8Na5Dq4ykRy32GsfFZ6IfJp8XyArURmZlj+XYFa5FqaMYRZRYMXzSsGuhfQUg2tEVd918AS6jPI9mn4ImS8TfCcKP4wM8O4gxRIvGyuw83DwxQtzyXgW141dW8vTTWta5etr3ejjjvwrQjpDuYbNgXAHcIJkEmacXV9TRZrA+5pFHVjiVwr6KdPFfRwCHpOok01Tsd4sI0oAmUKf3Mnlcufw2j/lA3vIMincG9wuZeDB6G4ISCNe10yiI2m8dbGrhaNUcm1bn4qzQp6pSRaqv009lu4+8aFnURuLv+NDKbpeU5kTH38MQe7mmBiCrdeQJpjLqHGoxmrullVm2jFPMzNFrRSHqb6xg1kN7qQbLgtwM03mShuKGobyOcPcT/V93x+IIc7c9h3Y/iDabfBJX0dZlxTN1hyraOfSvSwDGdnp0Smp8m4qV9XNzX7JIGw65BshEqTvWIpEvhZpIZu3oKpy0QsAsubYLgBK85KqK25HhoakuSnw2o4LNH7plmgNXWOqptwXx/8s7E02yTnujiPANMHLvlCHW43i0KWkzh8BnVDpWpZ1kNUPWRDhxSHUh+FAhQIU2Oymxk7Chsoo7gdMUvlYkdipoZtSEy7fiPde0vU8TLqnKoeIvGUpXvYit5fsaeaovs1NbBh3NDku7Vk4yduV40nMyRq4jGy9RPeJzJ4B8vVfll1W5uW6yEeYWg8oYefWmGUB1X3czWyb9BkQzSCO4o7bMnm2ijbaYtbVpMZj/YNJCpRt7a20WXVN3U0FsjwcABNjiITR0DInbpcjespTTVngxv3MsciIqIwWYrgpkY1yqtbUd2e6+gku4i1VA8H0Pv0+DRxbEUATTehaGrOpu4mYvJosJsSNQS7EnVnZyfuNFfRwxY0PQiJnWei9nizOhrVYewywdnl1e1Szc2//RZEs8Ob2LkgSkmTjdFSzdhEzWuEukPgzer3KlG3aOrHj3/DAPNvtvLgZnKgCR4BwTdzc7VAS7XINnMrcu0x13X+rUrVrVw9yNQW8xNHSzQ5/QFPU5hjuwIVNFcH2QqdxA0jFHZDKXWLqX5sVT/h6H2JfqeiZxW0KOwehd1xo5Ig7DLqBkM9GKJ+YqLfM7Jm1tCE3dND1cDu6oJXANmlRBXqFqJuV9WPTfQTgRaHIFnRQbVkd3d3lQ3BvqpaJz+x1HRQrWyf6ZfJ7rl7p7scnH/rh9SPpfqJBc2P51HQszp6XFX39NyRbmLvplAyZ6T8SrxbL69uMdWU/fg3BHMzRcvDm3hLVHZCT2ub78YFu4exSXQL4h09eO13XEkt0BBPZKg1rRwYM6uGjmbq/r4eHndKRhXqbAn1ExN9oBUHHhizXALN1QrbCudf1tTN1akHrWqHnhOpV8e7UHOBLdgf7R9FtcpW7frXrqruVNVmpg/MkjbQuA1MZ8PaBkT345PhnrJx5+4PqgeD6IODgFkpjwKmGnYm0e2CBb43ggR7nl1WLUZZHahurUp9y6J2yIGc/OA3iZZquQmMuDX1KFuhUUpMrkRcjbeYra0MXU4tyxrVKvrAjhbmfklWMg3ql9L8wrD30ReJnt5edUSLt/OtlF1C3VJa7ZRLtN/Pcl2g29d4otmCUBu6TwuG5rcPoG6rQi0KZFCaf2GZpmfqBdHL/V73srrJWGTahhYrNgwz3qiqahzLtVSrlmYdvcHJnF3Y6kp1bRYs5nG72Yqmky8i2XDsVCvZYULU2RJqViC3NPUvHH2gZlqaZ2e3r6f87Ul1G6me6JdlyH29fff5PBdVd3A1YYeqWxoCqQbtL0hm6I1AdaB5eXtrshtusLsLm5uLIeVRJtHkadU9i7qdqVvC1cFU/8LDOTDUsjp6u8S8wI2eQkUlHRRrasru6OjsJOqWytSIvhWO1tU91+VkRndhyl4eL8PMFjJT37zZydTlc83rIxytVDSWx9b2VoGWx/rmGjNPlKrp8uaq1fb6cEKqY5keHbPtp/wtUdBTE6USbSObZqK+KdUlKwTbIkffCqAP2BmieOqiVIN5crMrdX2THKdAzieAXTKm+qWCfmGsjbGYq1WrRf2rilYOa91SD6oD9Xqv171O9vcz8wRzh2b6/v1y6ntS3V5O3aqhUQ3vfiVofk7yhy0VvUaOMfH7l7iZqfVEC3elZFTfLKsmT0njToMF/StBH2wrx7SqecbonlzkZ21Is8YmAyd4dz8Y+LynMnWLoU6jOh6tUWv6Vx4a+oNEr/NM43sNzdhlzez5Nn1SpYqH4O1xGXWGPI6GVHshaO14Z2GGw6X4QTdTeqqN+hgbg+XOo2OhZk4mH9jsLYzpVXWrqc5E6NmdsH4A0bcI+raJJmyOXifvhHnxraU+AhusxqxmJdkP5DQ5UQ8Gc93C1OLA0Vj0WitNtI5WjywnZ57isXrLeJ6eRL8NlIcpNswPgnFv6IGCBvXgYCDXLVwdF+gaVh23bt8OosVpyXAO5/IyPQNwiZvfcrbcpjtumFX1A6tZDURTtl0tIpJm1WFDyzOeN8VJnHBkk0QjW9laDMkeG3tZuXnICKLGi5xNLQMa4q3OELR6LvXmuoF+K9BTunnsR8ymWrIbVHWs1WImaPUobXlMKy2PpSWLGWPMpn5gDwuZFwhVdxpq1uElHP8WNQfQ5onlHM1Pe1vg7ImJcHYJ89DQiA0t1INSzfo+PPk3kUhEHK9TJlqgHz1yQs0UvbiEp8SUS/Ur2HFwHxfhBmNkZGhoKDTVmpqwcddai4sLG2B1OC9oFf0I0KaZqde4einEPKFvCRPLswNLAcPMqvoWZzN3a4vv+S3tmvmRMFN04ChticZD0+zlQbBjr3QzX24pVk3xtQQWM0WbahG3BJmZQf2ImAk6eJS2Zub1AWGaefxshLak7ulTK5mjmTrI1sy3wUrFBL1lMa/zwzg5mrK1TFdsVsXcPGymGtSEzeCdnYIcND9yLKUhzJhp5dCmMHMAPTKiL6nT8zwMD1DIs5/Hjw01Y3eqZGFWwiljlmhZHzOvjKjGDGBKJuYn8D9VHQyLmaNVsjzMl51eqKDJaTelyabZIA+TTKsP2Eqpb98OR9vNS2pNEzU7oefVTLjaTLOEE6j2+FVOhA4OWt12M0UHS0M0w1WtOvhBSDMh5pGRMDNjBp4Ym+rB8mSC3rSa5amcUm0jq+gwc3BBj2mGicXBQRV+W8QjG7qcWaAXlFNjZvQ9xnQTW4j5aUXmXyT7dkkxokPK2SQb5hnF/PNVzE+MoLMwv8Ld62AZsUCXNy9YzHwfN90rOKKt368mzcJMZzXYGENctEPQ9iZYwgwHx8i952KTMZdXbxZoRf1ruNqpoAkGyDOvtbCblfXOlZt1dHim13/QrO4jfa5u7AByhXl+8suTis1Q6bedqsvZQIeSw/L8pMLasKPR/MipupyN2ngdan5WrTlQ0o/sqaaZrqY0TPPr6hL9o2bMNEH/iPn5v23GDtz5X5mf/XtmQF+9q/u3SwPVFP0DTfDfNrM83779/6RyK7vw9//aAAAAAElFTkSuQmCC">
<link rel="icon" type="image/png" href="/icono.png">
<link rel="manifest" href="/manifest.json">
<title>Mind Search</title>
<style>
 *{box-sizing:border-box;-webkit-tap-highlight-color:transparent}
 html,body{margin:0;height:100%;background:#fff;color:#1c1c1e;font-family:-apple-system,"Segoe UI",Roboto,Arial,sans-serif}
 #barra{position:fixed;top:0;left:0;right:0;height:54px;padding-top:env(safe-area-inset-top);display:flex;align-items:center;justify-content:space-between;background:#fff;border-bottom:1px solid #eee;z-index:2}
 #atras{color:#e0a800;font-size:17px;padding:0 14px}
 #titulo{font-weight:600;font-size:17px;padding:10px 20px;user-select:none}
 #listo{color:#e0a800;font-size:17px;font-weight:600;padding:14px 16px;cursor:pointer;user-select:none}
 #listo:active{opacity:.4}
 #hoja{position:absolute;top:54px;left:0;right:0;bottom:0;padding:14px 18px;overflow:auto}
 #fecha{color:#8e8e93;font-size:13px;text-align:center;margin:2px 0 12px}
 #nota{width:100%;min-height:75vh;border:0;outline:0;resize:none;font-size:18px;line-height:1.5;font-family:inherit;color:#1c1c1e;background:transparent}
 #guardado{position:fixed;bottom:18px;left:0;right:0;text-align:center;color:#8e8e93;font-size:13px;opacity:0;transition:opacity .3s;pointer-events:none}
 #guardado.ver{opacity:1}
 #ajustes{display:none;position:fixed;inset:0;background:rgba(0,0,0,.35);z-index:5;align-items:flex-end}
 #panel{background:#f2f2f7;width:100%;border-radius:16px 16px 0 0;padding:16px 16px 28px;max-height:88vh;overflow-y:auto;box-sizing:border-box}
 #panel h3{margin:4px 0 12px;font-size:16px}
 .fila{display:flex;gap:8px;margin:8px 0}
 .fila button{flex:1}
 #panel button{font-size:15px;padding:12px;border:0;border-radius:10px;background:#fff;color:#1c1c1e}
 #panel button.on{background:#e0a800;color:#fff}
 #panel label{font-size:14px;color:#555}
 #est{font-size:14px;margin:6px 0 10px}
 #cerrar{width:100%;margin-top:10px;background:#1c1c1e!important;color:#fff!important}
 #nuevo,#probar{width:100%;margin-top:8px}
</style></head><body>
<div id="barra">
 <div id="atras">&#x2039; Carpetas</div>
 <div id="titulo">Notas</div>
 <div id="listo">OK</div>
</div>
<div id="hoja">
 <div id="fecha"></div>
 <textarea id="nota" placeholder="Escribí una nota..." autocapitalize="sentences" autocomplete="off" spellcheck="false"></textarea>
</div>
<div id="guardado">Guardado &#10003;</div>

<div id="ajustes"><div id="panel">
 <h3>Ajustes</h3>
 <div id="est">...</div>
 <label>Qué abre en el espectador</label>
 <div class="fila">
  <button id="m_web" onclick="modo('web')">Google</button>
  <button id="m_img" onclick="modo('img')">Fotos</button>
  <button id="m_txt" onclick="modo('txt')">Solo texto</button>
 </div>
 <label>Celular del espectador</label>
 <div class="fila">
  <button id="c_and" onclick="cel(false)">Android</button>
  <button id="c_iph" onclick="cel(true)">iPhone</button>
 </div>
 <label>Cómo ir a la barra (Android) — probá cuál anda en Chrome</label>
 <div class="fila">
  <button id="b_1" onclick="barra(1)">Ctrl+L</button>
  <button id="b_2" onclick="barra(2)">Alt+D</button>
  <button id="b_3" onclick="barra(3)">Tecla Buscar</button>
 </div>
 <label>Abrir el navegador solo (Android)</label>
 <div class="fila">
  <button id="n_on" onclick="nav(true)">Sí</button>
  <button id="n_off" onclick="nav(false)">No (ya está abierto)</button>
 </div>
 <label>Tiempo de carga: <b id="cv"></b> s</label>
 <input id="carga" type="range" min="1" max="8" step="0.5" style="width:100%">
 <div class="fila">
  <button id="a_on" onclick="aviso(true)">Avisar "Guardado"</button>
  <button id="a_off" onclick="aviso(false)">No avisar</button>
 </div>
 <button id="probar" onclick="probar()">Probar (manda "prueba")</button>
 <div id="res" style="font-size:13px;color:#555;margin:6px 0 0"></div>
 <h3 style="margin-top:18px">Lapicera</h3>
 <div id="pst" style="font-size:14px;margin:4px 0 8px">...</div>
 <div style="background:#fff;border-radius:10px;padding:10px;margin:6px 0">
  <div style="font-size:14px">Última palabra: <b id="ulw">—</b></div>
  <div style="font-size:13px;color:#666;margin-top:4px">Otras opciones (tocá una para mandarla):</div>
  <div id="ulo" class="fila" style="flex-wrap:wrap;margin-top:4px"></div>
 </div>
 <div class="fila">
  <button id="l_on" onclick="lap('on=1')">Activada</button>
  <button id="l_off" onclick="lap('on=0')">Apagada</button>
 </div>
 <label>Pausa para dar la palabra por terminada: <b id="pv"></b> s</label>
 <input id="pausa" type="range" min="1" max="6" step="0.5" style="width:100%">
 <div class="fila">
  <input id="lpw" type="password" placeholder="Clave de la lapicera (si tiene)" style="flex:1;font-size:15px;padding:10px;border:0;border-radius:10px">
  <button onclick="lap('pw='+encodeURIComponent(document.getElementById('lpw').value))">Guardar</button>
 </div>
 <button onclick="lap('olvidar=1')" style="width:100%">Olvidar lapicera (vincular otra)</button>
 <label style="display:block;margin-top:10px">WiFi con internet (hotspot de tu celu o del lugar)</label>
 <div id="inet" style="font-size:14px;margin:4px 0">...</div>
 <div class="fila"><input id="wss" type="text" placeholder="Nombre del WiFi" autocapitalize="none" style="flex:1;font-size:15px;padding:10px;border:0;border-radius:10px"></div>
 <div class="fila">
  <input id="wsp" type="password" placeholder="Clave" style="flex:1;font-size:15px;padding:10px;border:0;border-radius:10px">
  <button onclick="wifi()">Guardar</button>
 </div>
 <button id="nuevo" onclick="nuevo()">Nuevo espectador</button>
 <button id="cerrar" onclick="cerrarAj()">Cerrar</button>
</div></div>

<script>
let M='web',IP=false,CARGA=2.5,AV=true,NAV=true,BAR=1,ultimaCant=0,esperandoId=0;
const nota=document.getElementById('nota');
try{
 M=localStorage.getItem('m')||'web';if(['web','img','txt'].indexOf(M)<0)M='web';IP=localStorage.getItem('i')==='1';
 CARGA=parseFloat(localStorage.getItem('c')||'2.5');AV=localStorage.getItem('a')!=='0';NAV=localStorage.getItem('n')!=='0';BAR=parseInt(localStorage.getItem('b')||'1');
 nota.value=localStorage.getItem('nota')||'';
}catch(e){}
ultimaCant=(nota.value.match(/\n/g)||[]).length;

function guardar(){try{localStorage.setItem('m',M);localStorage.setItem('i',IP?'1':'0');localStorage.setItem('c',CARGA);localStorage.setItem('a',AV?'1':'0');localStorage.setItem('n',NAV?'1':'0');localStorage.setItem('b',BAR);localStorage.setItem('nota',nota.value)}catch(e){}}
function pintar(){
 ['web','img','txt'].forEach(x=>document.getElementById('m_'+x).className=(M==x?'on':''));
 document.getElementById('c_and').className=IP?'':'on';
 document.getElementById('c_iph').className=IP?'on':'';
 document.getElementById('a_on').className=AV?'on':'';
 document.getElementById('a_off').className=AV?'':'on';
 [1,2,3].forEach(x=>document.getElementById('b_'+x).className=(BAR==x?'on':''));
 document.getElementById('n_on').className=NAV?'on':'';
 document.getElementById('n_off').className=NAV?'':'on';
 document.getElementById('carga').value=CARGA;document.getElementById('cv').textContent=CARGA;
}
function aESP(){fetch('/ajustes?m='+M+'&i='+(IP?1:0)+'&n='+(NAV?1:0)+'&b='+BAR+'&w='+Math.round(CARGA*1000)).catch(()=>{})}
function modo(x){M=x;guardar();pintar();aESP()}
function cel(x){IP=x;guardar();pintar();aESP()}
function aviso(x){AV=x;guardar();pintar()}
function nav(x){NAV=x;guardar();pintar();aESP()}
function barra(x){BAR=x;guardar();pintar();aESP()}
function lap(q){fetch('/lapicera?'+q).then(()=>setTimeout(estado,600)).catch(()=>{})}
function wifi(){const s=document.getElementById('wss').value,p=document.getElementById('wsp').value;
 fetch('/wifi?s='+encodeURIComponent(s)+'&p='+encodeURIComponent(p)).then(()=>{document.getElementById('inet').textContent='Conectando a '+s+'...';setTimeout(estado,5000)}).catch(()=>{})}
document.getElementById('pausa').oninput=e=>{document.getElementById('pv').textContent=e.target.value};
document.getElementById('pausa').onchange=e=>lap('pau='+Math.round(parseFloat(e.target.value)*1000));
document.getElementById('carga').oninput=e=>{CARGA=parseFloat(e.target.value);guardar();pintar()};
document.getElementById('carga').onchange=()=>aESP();

// fecha como en una app de notas
const f=new Date();document.getElementById('fecha').textContent=
 f.toLocaleDateString('es-AR',{day:'numeric',month:'long',year:'numeric'})+', '+f.toLocaleTimeString('es-AR',{hour:'2-digit',minute:'2-digit'});

// 3 toques en "Notas" = ajustes secretos
let toques=0,tt=0;
document.getElementById('titulo').addEventListener('click',()=>{
 toques++;clearTimeout(tt);tt=setTimeout(()=>toques=0,600);
 if(toques>=3){toques=0;document.getElementById('ajustes').style.display='flex';pintar();estado()}
});
function cerrarAj(){document.getElementById('ajustes').style.display='none'}
// "OK" de arriba = mandar la línea donde está el cursor (o la última escrita)
function lineaActual(){
 const v=nota.value,c=(document.activeElement===nota)?nota.selectionStart:v.length;
 let ini=v.lastIndexOf('\n',c-1)+1,fin=v.indexOf('\n',c);if(fin<0)fin=v.length;
 let l=v.substring(ini,fin);
 if(!limpiar(l)){const ls=v.split('\n').filter(x=>limpiar(x));l=ls.length?ls[ls.length-1]:''}
 return l;
}
document.getElementById('listo').addEventListener('pointerdown',e=>{
 e.preventDefault();               // que no se cierre el teclado antes de leer el cursor
 const w=limpiar(lineaActual());
 if(w)enviar(w);
 nota.blur();
});

// pantalla completa al primer toque (Android)
document.addEventListener('touchend',function(){
 const d=document.documentElement;
 if(!document.fullscreenElement&&d.requestFullscreen){d.requestFullscreen().catch(()=>{})}
},{once:true});

function limpiar(s){
 return s.normalize('NFD').replace(/[̀-ͯ]/g,'').replace(/ñ/gi,'n')
  .replace(/[^a-zA-Z0-9 .,'-]/g,'').replace(/\s+/g,' ').trim();
}

// ENTER en la nota = mandar la línea que se acaba de escribir
nota.addEventListener('input',()=>{
 guardar();
 const cant=(nota.value.match(/\n/g)||[]).length;
 if(cant>ultimaCant){
  const antes=nota.value.substring(0,nota.selectionStart).split('\n');
  const linea=antes.length>=2?antes[antes.length-2]:'';
  const w=limpiar(linea);
  if(w)enviar(w);
 }
 ultimaCant=cant;
});

function avisoAbajo(txt,ms){
 const g=document.getElementById('guardado');g.textContent=txt;g.classList.add('ver');
 setTimeout(()=>g.classList.remove('ver'),ms||2500);
}
const MOTIVO={bt:'Espectador NO conectado por Bluetooth',ocupado:'Todavía estaba escribiendo la anterior',vacio:'Palabra vacía'};
function enviar(w){
 document.getElementById('guardado').classList.remove('ver');
 document.getElementById('res').textContent='Enviando "'+w+'"...';
 fetch('/enviar?t='+encodeURIComponent(w)+'&m='+M+'&i='+(IP?1:0)+'&n='+(NAV?1:0)+'&b='+BAR+'&w='+Math.round(CARGA*1000))
  .then(r=>r.json()).then(j=>{
   if(j.ok){esperandoId=j.id;seguir();document.getElementById('res').textContent='Escribiendo "'+w+'" en el espectador...'}
   else{document.getElementById('res').textContent='No se mandó: '+(MOTIVO[j.r]||'error');
        avisoAbajo('No se pudo sincronizar',3000);if(navigator.vibrate)navigator.vibrate([60,80,60])}
  }).catch(()=>{document.getElementById('res').textContent='No llego a la ESP32 (¿WiFi TP-Link_6758 conectado?)';
        avisoAbajo('Sin conexión',3000)});
}
function probar(){enviar('prueba')}

// pregunta a la ESP32 hasta que la foto esté cargada
function seguir(){
 fetch('/estado').then(r=>r.json()).then(j=>{
  if(j.id!==esperandoId)return;
  if(j.f===3){
   esperandoId=0;
   document.getElementById('res').textContent='Listo: ya debería estar en pantalla';
   if(AV){avisoAbajo('Guardado \u2713',2500);if(navigator.vibrate)navigator.vibrate(60)}
  }else if(j.f===1||j.f===2){setTimeout(seguir,300)}
 }).catch(()=>setTimeout(seguir,600));
}

const PEST=['Apagada','Buscando la lapicera... (prendela)','Conectando...','Preparando...','Lista ✔','Pide clave: escribila abajo'];
function estado(){
 fetch('/estado').then(r=>r.json()).then(j=>{
  document.getElementById('est').textContent=j.c?'Espectador conectado ✔':'Espectador NO conectado';
  let t=j.pe?PEST[j.p]||'':'Apagada';
  if(j.pe&&j.p===4&&j.pb>=0)t+='  ·  batería '+j.pb+'%';
  if(j.er)t+='  ·  '+j.er;
  document.getElementById('pst').textContent=t;
  document.getElementById('ulw').textContent=j.lw||'—';
  const ul=document.getElementById('ulo');const ops=(j.lo||'').split('|').filter(x=>x);
  if(ul.dataset.v!==j.lo){ul.dataset.v=j.lo||'';ul.innerHTML='';
   if(!ops.length)ul.innerHTML='<span style="font-size:13px;color:#999">—</span>';
   ops.forEach(w=>{const b=document.createElement('button');b.textContent=w;b.onclick=()=>lap('mandar='+encodeURIComponent(w));ul.appendChild(b)})}
  document.getElementById('l_on').className=j.pe?'on':'';
  document.getElementById('l_off').className=j.pe?'':'on';
  document.getElementById('pausa').value=(j.pau/1000);document.getElementById('pv').textContent=(j.pau/1000);
  document.getElementById('inet').textContent=j.in?'Internet ✔ ('+j.ss+')':(!j.ss?'Sin WiFi configurado':(j.sb?'Buscando '+j.ss+'...':(j.pe?'Sin internet ahora (se conecta solo cuando escriben). ¿Está prendido '+j.ss+'?':'Se usa solo con la lapicera activada')));
  if(j.ss&&!document.getElementById('wss').value)document.getElementById('wss').value=j.ss;
 }).catch(()=>document.getElementById('est').textContent='Sin conexión con la ESP32');
}
function nuevo(){
 if(!confirm('¿Cortar el celular actual y dejar libre para otro?'))return;
 fetch('/nuevo').then(()=>setTimeout(estado,1500)).catch(()=>{});
}
setInterval(()=>{if(document.getElementById('ajustes').style.display==='flex')estado()},2000);
pintar();
</script></body></html>)HTML";

const char MANIFEST[] PROGMEM = R"JSON({
 "name":"Mind Search","short_name":"Mind Search","start_url":"/","scope":"/",
 "display":"standalone","background_color":"#ffffff","theme_color":"#ffffff",
 "icons":[{"src":"/icono.png","sizes":"180x180","type":"image/png","purpose":"any"}]
})JSON";
