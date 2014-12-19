![Spectrum Icon](https://spectrum.web.cern.ch/spectrum/img/spectrum_banner_512.png)
###Spectrum Documentation – v0.8
###19 December, 2014
###Authors: [Tancredi Carli](mailto:tancredi.carli@cern.ch), [Joe Gibson](mailto:gibsjose@mail.gvsu.edu), [Cameron Embree](csembree@gmail.com)

#Recommended Directory Structure

We have experimented with different directory structures, and have found that the following structure works quite well. Of course, you are free to use whichever structure you would like, but we feel this structure is easy to understand and intuitive.

```bash
> Spectrum <--- Spectrum root directory (where the executable is)
    > Data
        > <observable>
            > <experiment>
                > <specific observable>
                    > ___.txt
                    > ___.txt
                    > ...

    > Grids
        > <observable>
            > <experiment>
                > <nlo program>
                    > <specific observable>
                        > ___.txt
                        > ___.txt
                        > ...

    > PDF
        > ___.txt
        > ___.txt
        > ...

    > PDFsets
        > ...
```

So an example structure might look something like this:

```bash
> Spectrum
    > Data
        > jet
        > z
        > top
            > cms
            > atlas
                > ttbarresolvedpartons
                    > atlas_2011_top_partons_mtt.txt
                    > ...

    > Grids
        > jet
        > z
        > top
            > cms
            > atlas
                > mcfm
                > sherpa
                    > ttbarresolvedpartons
                        > atlas_2011_top_partons_mtt.txt

    > PDF
        > CT10.txt
        > HERAPDF15NLO.txt
        > CTEQ6M.txt
        > MRST2008nlo.txt
        > NNPDF23nlo.txt
        > NNPDF30nlo.txt

    > PDFsets
        > ...
```
