// Custom color palettes for plotting in ROOT
// Colors are defined as integers and described in the TColor documentation:
// https://root.cern.ch/doc/master/classTColor.html
/* Usage:
  #include Palettes.C
  
  int nColors = <int value >= 1>;
  int palette[nColors];
  fillPalette("<palette name>", nColors, palette);
  gStyle->SetPalette(nColors, palette);

*/

//Fill nColors from the predefifined palette "name" into palette
// name : the name of the palette to use
// nColors : the number of colors needed. If nColors > palette length fills all remaining colors as kBlack;
// palette : an array of length nColors to fill with colors
void fillPalette(TString name, int nColors, int palette[])
{ 
  //Palette optimized for non-filled plots
  const int N_LINE = 10;
  int colors_line[N_LINE] = {1, 2, 4, 6, 9, 12, 28, 32, 36, 634};

  //Large pallette optimized for filled plots
  const int N_STACK = 15;
  int colors_stack[N_STACK] = {602, 597, 595, 434, 426, 419, 411, 414, 402, 797, 626, 634, 610, 618, 619};

  //Cool tone color palette
  const int N_COOL = 12;
  int colors_cool[N_COOL] = {884, 874, 881, 876, 890, 855, 852, 867, 835, 844, 419, 413};

  //Warm tone color palette
  const int N_WARM = 9;
  int colors_warm[N_WARM] = {618, 617, 612, 628, 633, 804, 802, 807, 800};

  //Choose a palette
  int maxColors;
  int* colors;
  if (name == "line")
    {
      maxColors = N_LINE;
      colors = colors_line;
    }
  else if (name == "stack")
    {
      maxColors = N_STACK;
      colors = colors_stack;
    }
  else if (name == "cool")
    {
      maxColors = N_COOL;
      colors = colors_cool;
    }
  else if (name == "warm")
    {
      maxColors = N_WARM;
      colors = colors_warm;
    }
  else
    {
      cout << "\nWARNING: Palette name " << name << " does not match pre-defined palletes\n" << endl;
      maxColors = N_LINE;
      colors = colors_line;
    }

  //Check that requested number of colors is satisfied by the palette
  if (maxColors < nColors)
    {
      cout << "\nWARNING: " << nColors << " colors were requested but only " << maxColors << " are available\n" << endl;
    }

  //Fill the palette
  for (int i = 0; i < nColors; i++)
    {
      if (i < maxColors)
        palette[i] = colors[i];
      else
        palette[i] = 1;
    }

}

//Function for visualizing palettes
//Currently only supports filled area visualation
//TODO: Add line visualization support
// name : the name of the palette to visualize
// nColors : the number of colors of the palette to visualize
void testPalette(TString name, int nColors)
{
  int palette[nColors];
  fillPalette(name, nColors, palette);
  gStyle->SetPalette(nColors, palette);

  TCanvas *canv = new TCanvas("canv", "Palette Testing", 800, 600);

  TF2 *f1 = new TF2("f1","y",0, 1 ,1,10);
  f1->Draw("colz");
}


