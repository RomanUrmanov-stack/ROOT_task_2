{   
    //creating canvas and dividing it on four parts
    TCanvas* canvas = new TCanvas("canvas", "canvas", 200, 200, 500, 500);
    canvas->Divide(2,2);

    //opening .dat files
    string path_1 = "fdata.dat", path_2 = "fMC.dat";
    ifstream fin_1, fin_2;
    try
    {
        fin_1.open(path_1);
        fin_2.open(path_2);
    }
    catch(const exception& e)
    {
        cout << "file wasn't open" << endl;
        cerr << e.what() << '\n';
        return 1;
    }

    //dummy variables for histogram filling
    Int_t n_bins_x = 100, n_bins_y = 100, x_lower_limit = 60, x_upper_limit = 220, y_lower_limit = 60, y_upper_limit = 220;
    Double_t low_signal_x = 110., up_signal_x = 160., low_control_x = 100., up_control_x = 170.;
    Double_t m1, m2;
    
    //filling the m1:m2 histogram
    canvas->cd(1);  

    TH2D* hist_1 = new TH2D("hist_1", "hist_1; m2; m1", n_bins_x, x_lower_limit, x_upper_limit, n_bins_y, y_lower_limit, y_upper_limit); //m2 == x axis, m1 == y axis
    while(fin_1 >> m1 >> m2) hist_1->Fill(m1, m2);

    hist_1->Draw("lego");

    //m2 projection
    canvas->cd(2);
    hist_1->ProjectionX()->Draw("hist");
    hist_1->ProjectionX()->SetTitle("m2 experimental data");

    //simulation histogram
    canvas->cd(3);

    //histogram with singal zone events of m1
    TH2D* hist_2_temp = new TH2D("hist_2_temp", "hist_2_temp", n_bins_x, x_lower_limit, x_upper_limit, n_bins_y, y_lower_limit, y_upper_limit);

    for(int i = hist_1->ProjectionX()->FindBin(low_signal_x); i < hist_1->ProjectionX()->FindBin(up_signal_x); i++)
    {
        for (int j = hist_1->ProjectionY()->FindFirstBinAbove(0); j < hist_1->ProjectionY()->FindLastBinAbove(0); j++)
        {
            hist_2_temp->SetBinContent(i, j, hist_1->GetBinContent(i, j));
        }
    }

    TH1D* hist_2 = (TH1D*)hist_2_temp->ProjectionY();

    hist_2->Draw("hist");
    hist_2->SetFillStyle(3001);
    hist_2->SetFillColor(kBlue);

    //histogram with control zone events of m1
    Double_t h3_scale = (Double_t)(up_signal_x - low_signal_x)/(low_control_x - x_lower_limit + x_upper_limit - up_control_x); //creating a scale for control zone histogram 

    TH2D* hist_3_temp = new TH2D("hist_3_temp", "hist_3_temp", n_bins_x, x_lower_limit, x_upper_limit, n_bins_y, y_lower_limit, y_upper_limit);

    //filling events from lower band
    for(int i = hist_1->ProjectionX()->FindFirstBinAbove(0); i < hist_1->ProjectionX()->FindBin(low_control_x); i++)
    {
        for (int j = hist_1->ProjectionY()->FindFirstBinAbove(0); j < hist_1->ProjectionY()->FindLastBinAbove(0); j++)
        {
            hist_3_temp->SetBinContent(i, j, hist_1->GetBinContent(i, j));
        }
    }

    //filling events from higher band
    for(int i = hist_1->ProjectionX()->FindBin(up_control_x); i < hist_1->ProjectionX()->FindLastBinAbove(0); i++)
    {
        for (int j = hist_1->ProjectionY()->FindFirstBinAbove(0); j < hist_1->ProjectionY()->FindLastBinAbove(0); j++)
        {
            hist_3_temp->SetBinContent(i, j, hist_1->GetBinContent(i, j));
        }
    }

    //creating m1 spectrum
    TH1D* hist_3 = (TH1D*)hist_3_temp->ProjectionY();

    //scaling histogram
    hist_3->Scale(h3_scale);
    
    hist_3->Draw("samehist");
    hist_3->SetFillStyle(3001);
    hist_3->SetFillColor(kRed);

    //simulation histogram
    TH2D* hist_4_temp = new TH2D("hist_4_temp", "hist_4_temp", n_bins_x, x_lower_limit, x_upper_limit, n_bins_y, y_lower_limit, y_upper_limit); //m3 == y axis, m4 == x axis
    while(fin_2 >> m1 >> m2) hist_4_temp->Fill(m1, m2);

    TH1D* hist_4 = (TH1D*)hist_4_temp->ProjectionY();

    hist_4->SetNormFactor(hist_2->GetEntries() - hist_3->GetEntries()); //normalizing simulation histogram to the number of singal events in real experiment

    TH1D* hist_5 = new TH1D("hist_5", "hist_5", n_bins_y, y_lower_limit, y_upper_limit);

    hist_5->Add(hist_3, hist_4);
    
    hist_5->Draw("histsame");
    hist_5->SetFillStyle(3001);
    hist_5->SetFillColor(kYellow);

    //closing .dat files
    fin_1.close();
    fin_2.close();
}