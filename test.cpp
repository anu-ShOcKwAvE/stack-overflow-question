
#include <blazeContainer.hpp>
using namespace blaze;

int main()
{
uvector2d<int> vect(3,3){ { 1, 2, 3 },
                           { 4, 5, 6 }, 
                           { 7, 8, 9 } };
 // This is what i actually wanted to do initialize [0][1] with 1
 //[0][2] with 2 ......... and [2][2] with 9
return 0;
}
