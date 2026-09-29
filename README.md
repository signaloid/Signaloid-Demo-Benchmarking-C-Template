[<img src="https://assets.signaloid.io/add-to-signaloid-cloud-logo-dark-latest.png#gh-dark-mode-only" alt="[Add to signaloid.io]" height="30">](https://signaloid.io/repositories?connect=https://github.com/signaloid/Signaloid-Demo-TemplateWithSingleMains#gh-dark-mode-only)
[<img src="https://assets.signaloid.io/add-to-signaloid-cloud-logo-light-latest.png#gh-light-mode-only" alt="[Add to signaloid.io]" height="30">](https://signaloid.io/repositories?connect=https://github.com/signaloid/Signaloid-Demo-TemplateWithSingleMains#gh-light-mode-only)


# Internal Template for Demos in Benchmarking Format
This template is for creating demonstration
application repositories that adhere to the structure required by the automated benchmarking tools found in the [Signaloid Python package](https://github.com/signaloid/signaloid-python/tree/main/src/signaloid/benchmarking/automation).

## Cloning the repository
The correct way to clone this repository is:
```
git clone --recursive https://github.com/signaloid//Signaloid-Demo-Benchmarking-C-Template.git
```
To update all submodules
```
git pull --recurse-submodules
git submodule update --remote --recursive
```
If you forgot to clone with `--recursive`, and end up with empty submodule directories, you can remedy this with
```
git submodule update --init --recursive
```


## Development workflow
Develop in a development branch separate from the main branch. This will allow you to open a 
pull request to merge into the main branch, to aid the code review process.

## Provide input for public documentation
As the person implementing the example, please provide two short paragraphs of what the example does:
1. First paragraph (**Grandmother's Overview**): A lay-person's overview of (a) why the example matters (b) how the example works today (c) how uncertainty-tracking makes the example better.

2. Second paragraph (**Technical Overview**): A brief technical overview of (a) why the example matters (b) how the example works today (c) how uncertainty-tracking makes the example better.


## Configure the "add to signaloid.io" snippet
Edit the snippet at the top of this `README.md`, whose source is repeated in the block below, to include the URL of your Git repository (replacing the text `<your repository URL here>`)[^1]:
```html
[<img src="https://assets.signaloid.io/add-to-signaloid-cloud-logo-dark-v6.svg#gh-dark-mode-only" alt="[Add to signaloid.io]" height="30">](https://signaloid.io/repositories?connect=<your repository URL here>#gh-dark-mode-only)
[<img src="https://assets.signaloid.io/add-to-signaloid-cloud-logo-light-v6.svg#gh-light-mode-only" alt="[Add to signaloid.io]" height="30">](https://signaloid.io/repositories?connect=<your repository URL here>#gh-light-mode-only)
```


## Release a repository made from this template
You should first develop and test the repositories made from this template as a private repository.

#### Code review:
1. Open a pull request (PR) to invite others to review the code in the private repository.
2. For the PR, request review from three team members. This avoids having every individual look at every example, taking up the team's time.
3. Reviewers can comment on code in the PR, suggest or push changes etc.
4. Once all reviewers are happy, the PR can be merged.

#### Warning. To make the example public once it is ready:
1. Create a new private bare repository on GitHub. Name the repository `Signaloid-Demo-Topic-UpperCamelCasedDescription` (e.g., `Signaloid-Demo-Metallurgy-BrownHamModel`). We use the prefix `Signaloid-Demo-` to highlight provenance even on a clone.
2. Copy over a clone of the example repository and delete its `.git` folder.
3. Delete **all** the content of this `README.md`, replace it with appropriate new content, and remember to add the code snippet, appropriately edited to connect the new repository's URL to the Signaloid Cloud platform.
4. Perform a `git init` on the copied clone.
5. Push the history-free example repository to the private GitHub repository created in Step 1 above.
6. Have two other people in the team review the private repository before making it public and **make sure the repository you are about to make public has no commit history other than the initial import message**.

<br/>
<br/>
<br/>

[^1]:
    :construction: &nbsp; **The syntax of the snippet:**
    1. In the snippet, the anchors `#gh-dark-mode-only` and `#gh-light-mode-only` are both on the URL as well as on the image. The GitHub documentation officially implies you should place the anchor at the end of the image name, which works fine if the image is in your repository. For external images, GitHub currently copies the image to `camo.githubusercontent.com` and the version in the rendered Markdown loses the anchor in the process.
    2. The snippet mixes HTML and Markdown in order to allow resizing the image.
